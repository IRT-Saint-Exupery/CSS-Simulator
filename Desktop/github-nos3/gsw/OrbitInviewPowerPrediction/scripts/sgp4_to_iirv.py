#!/usr/bin/env python

import argparse
from argvalidator import ArgValidator
from datetime import datetime, timedelta
import requests
from fileinput import input
import pandas as pd
import math
from sgp4.api import Satrec
from pyorbital.orbital import astronomy

###############################################################################
# Script to use the sgp4 module to compute IIRVs for a satellite number
# (default is 43852, which is STF-1)
###############################################################################

class SatelliteTleException(BaseException):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class SatelliteTle:
    """Class to retrieve and use a two line element set for a given satellite """
    # Constructor
    def __init__(self, satellite_number, satellite_name = None, satellite_contact_name = None, tle_url = "http://www.celestrak.com/NORAD/elements/cubesat.txt", tle_file = None):
        """Constructor:  satellite number according to NORAD"""
        self.__satellite_number = satellite_number
        self.__tle_url = tle_url
        self.__tle_file = tle_file # Like "C:\Users\msuder\Desktop\STF1-TLE.txt"
        self.__satellite_name = satellite_name
        self.__satellite_contact_name = satellite_contact_name
        self.__line1 = None
        self.__line2 = None
        self.__get_tle()

    # Private method to fetch the TLE
    def __get_tle(self):
        """Method to retrieve the TLE for the initialized satellite number, usually from Celestrak (could be from a hardwired file).  No return value."""
        if (self.__tle_file is not None):
            lines = input(self.__tle_file)
        else:
            try:
                lines = requests.get(self.__tle_url).text.splitlines() 
            except:
                default_file = "C:/Users/msuder/Desktop/cubesat.txt"
                lines = input(default_file)

        last_name = ""
        for line in lines:
            if "1 %s" % self.__satellite_number in line:
                self.__line1 = line[0:69]
                if (self.__satellite_name is None):
                    self.__satellite_name = last_name
            if "2 %s" % self.__satellite_number in line:
                self.__line2 = line[0:69]
            if ("1 " != line[0:2]) and ("2 " != line[0:2]):
                last_name = line.rstrip()
            else:
                last_name = None
                
        if ((self.__line1 is None) or (self.__line2 is None)):
            if (self.__tle_file is not None):
                raise SatelliteTleException('Could not find TLE for satellite %s in file %s' % \
                    (self.__satellite_number, self.__tle_file))
            else:
                raise SatelliteTleException('Could not find TLE for satellite %s at URL %s' % \
                    (self.__satellite_number, self.__tle_url))
        else:
            self.__satellite = Satrec.twoline2rv(self.__line1, self.__line2)
    
    def compute_ephemeris_point(self, in_time):
        """Method to compute an ephemeris point for a given time"""
        time = pd.Timestamp(in_time).to_julian_date()
        (e, pos, vel) = self.__satellite.sgp4(time, 0.0)
        return (in_time, pos, vel)

    def compute_ephemeris_table(self, in_start_time, in_end_time, time_step_seconds):
        """Method to compute a table of ephemerides for a given time span at a given time step"""
        time = in_start_time
        try:
            delta = timedelta(seconds=time_step_seconds)
        except:
            delta = timedelta(seconds=60)
        ephem_tbl = []
        while (time < in_end_time + delta):
            ephem_tbl.append(self.compute_ephemeris_point(time))
            time += delta

        return ephem_tbl

def main():
    """Main function... makes 'forward declarations' of helper functions unnecessary"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--satnum", help="Specify satellite number (e.g. 25544=ISS, 43852=STF-1)", \
            type=int, metavar="[1-99999]", choices=range(1,99999), default=43852)
    parser.add_argument("-t", "--time", help="Specify date/time (UTC)", type=ArgValidator.validate_datetime, metavar="YYYY-MM-DDTHH:MM:SS", default=datetime.now())
    parser.add_argument("-r", "--endtime", help="Specify date time range with this end date/time (UTC)", \
            type=ArgValidator.validate_datetime, metavar="YYYY-MM-DDTHH:MM:SS", default=None)
    parser.add_argument("-d", "--timestep", help="Specify time step (delta) in seconds for tabular data", \
            type=int, metavar="[1-86400]", choices=range(1,86400), default=60)
    parser.add_argument("-f", "--file", help="TLE file to use (instead of looking up the TLE on CelesTrak)", type=ArgValidator.validate_file, default=None)
    parser.add_argument("-v", "--iirv", help="Print improved interrange vector (IIRV) format", action="store_true")
    args = parser.parse_args()

    if (args.file is not None):
        st = SatelliteTle(args.satnum, tle_file=args.file)
    else:
        saturl = "http://www.celestrak.com/cgi-bin/TLE.pl?CATNR=%s" % args.satnum
        st = SatelliteTle(args.satnum, tle_url=saturl)

    if (args.iirv):
        if (args.endtime is None):
            point = st.compute_ephemeris_point(args.time)
            print_iirv_point(st, point)
        else:
            table = st.compute_ephemeris_table(args.time, args.endtime, args.timestep)
            print_iirv_points(st, table)

def print_iirv_point(st, point):
    print("GIIRV MANY\r\r\n")
    tt = point[0].timetuple()
    string = "1211800001000%3.3d%2.2d%2.2d%2.2d%3.3d" % (tt.tm_yday, tt.tm_hour, tt.tm_min, tt.tm_sec, int(point[0].microsecond/1000.0))
    csum = checksum(string)
    print("%s%3.3d\r\r\n" % (string, csum))
    gmst_radians = astronomy.gmst(point[0])
    #print("gmst_radians=%s, degrees=%s" % (gmst_radians, gmst_radians * 180.0/3.1415927))
    (x, y, z) = (point[1][0], point[1][1], point[1][2])
    r = math.sqrt(x*x + y*y)
    theta = math.atan2(y, x)
    x = r*math.cos(-1.0*gmst_radians+theta)
    y = r*math.sin(-1.0*gmst_radians+theta)
    string = "% 013.0f% 013.0f% 013.0f" % (x*1000.0, y*1000.0, z*1000.0)
    csum = checksum(string)
    print("%s%3.3d\r\r\n" % (string, csum))
    (x, y, z) = (point[2][0], point[2][1], point[2][2])
    r = math.sqrt(x*x + y*y)
    theta = math.atan2(y, x)
    x = r*math.cos(-1.0*gmst_radians+theta)
    y = r*math.sin(-1.0*gmst_radians+theta)
    string = "% 013.0f% 013.0f% 013.0f" % (x*1000000.0, y*1000000.0, z*1000000.0)
    csum = checksum(string)
    print("%s%3.3d\r\r\n" % (string, csum))
    mass = 4475570
    cross = 99999
    drag = 207
    solar = 0
    string = "%08.0f%05.0f%04.0f% 08.0f" % (mass, cross, drag, solar)
    csum = checksum(string)
    print("%s%3.3d\r\r\n" % (string, csum))
    print("ITERM GAQD\r\r\n")

def checksum(s):
    csum = 0
    for c in s:
        if (c == ' '):
            pass
        elif (c == '-'):
            csum = csum + 1
        else:
            csum = csum + int(c)
    return csum

def print_iirv_points(st, table):
    for i in range(0, len(table)):
        print("GIIRV MANY\r\r\n")
        tt = table[i][0].timetuple()
        string = "1111800001%3.3d%3.3d%2.2d%2.2d%2.2d%3.3d" % (i+1, tt.tm_yday, tt.tm_hour, tt.tm_min, tt.tm_sec, int(table[i][0].microsecond/1000.0))
        csum = checksum(string)
        print("%s%3.3d\r\r\n" % (string, csum))
        gmst_radians = astronomy.gmst(table[i][0])
        #print("gmst_radians=%s, degrees=%s" % (gmst_radians, gmst_radians * 180.0/3.1415927))
        (x, y, z) = (table[i][1][0], table[i][1][1], table[i][1][2])
        r = math.sqrt(x*x + y*y)
        theta = math.atan2(y, x)
        x = r*math.cos(-1.0*gmst_radians+theta)
        y = r*math.sin(-1.0*gmst_radians+theta)
        string = "% 013.0f% 013.0f% 013.0f" % (x*1000.0, y*1000.0, z*1000.0)
        csum = checksum(string)
        print("%s%3.3d\r\r\n" % (string, csum))
        (x, y, z) = (table[i][2][0], table[i][2][1], table[i][2][2])
        r = math.sqrt(x*x + y*y)
        theta = math.atan2(y, x)
        x = r*math.cos(-1.0*gmst_radians+theta)
        y = r*math.sin(-1.0*gmst_radians+theta)
        string = "% 013.0f% 013.0f% 013.0f" % (x*1000000.0, y*1000000.0, z*1000000.0)
        csum = checksum(string)
        print("%s%3.3d\r\r\n" % (string, csum))
        mass = 4544100
        cross = 99999
        drag = 200
        solar = 1500000
        string = "%08.0f%05.0f%04.0f% 08.0f" % (mass, cross, drag, solar)
        csum = checksum(string)
        print("%s%3.3d\r\r\n" % (string, csum))
        print("ITERM GAQD\r\r\n")
        
# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()
