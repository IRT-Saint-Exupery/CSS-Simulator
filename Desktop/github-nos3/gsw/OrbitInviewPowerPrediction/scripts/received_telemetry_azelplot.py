#!/usr/bin/env python
#
# Quick script to graph where STF-1 would be in azimuth and elevation relative to Wallops at times
# specified in an input file... if these files represent when telemetry was received, then the 
# plot indicates where STF-1 was when the telemetry was received.  The location computations are
# based on TLEs.
#
# Syntax:  received_telemetry_azelplot.py <file of times, format YYYY-MM-DD HH:MM:SS>
#

import json
import tempfile
from argvalidator import ArgValidator
import os
import sys
import glob
from configuration import Configuration
from pyorbital.orbital import Orbital
from satellite_tle import SatelliteTle
from ground_station import GroundStation
from az_el_range_report import AzElRangeReportGenerator
from datetime import datetime
from pytz import timezone, UTC
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

def main():
  conf_file = "config/sat_html_report.config"
  if (len(sys.argv) > 1):
      conf_file = sys.argv[1]
  #sys.stderr.write("conf_file: %s\n" % conf_file)
    
  with open(conf_file) as json_data_file:
      data = json.load(json_data_file)

  base_output_dir = Configuration.get_config_directory(data.get('base_output_directory',tempfile.gettempdir()))
  tz = Configuration.get_config_timezone(data.get('timezone','US/Eastern'))
  start_day = Configuration.get_config_int(data.get('start_day','0'), -180, 180, 0)
  end_day = Configuration.get_config_int(data.get('end_day','0'), -180, 180, 0)
  time_step_seconds = Configuration.get_config_float(data.get('time_step_seconds','15'), 1, 600, 15)

  #parser.add_argument("-f", "--file", help="File of times, format YYYY-MM-DD HH:MM:SS", type=ArgValidator.validate_file, default=None)
  #parser.add_argument("-d", "--directory", \
  #    help="Directory of dated directories containing TLE files (*.tle), assumed format of dated directory is YYYY-MM-DD", \
  #    type=ArgValidator.validate_directory, default="/home/itc/Desktop/oipp-data/stf1/")
  #parser.add_argument("-g", "--debug", help="Print debug information", action="store_true")
  #parser.add_argument("-t", "--table", help="Print table of time, azimuth, elevation information", action="store_true")
  #parser.add_argument("-m", "--summary", help="Print summary of days found and points per day", action="store_true")

  if (data['report_type'] == "AzElCmdTelem"):
      create_azel_cmd_telemetry_report(base_output_dir, tz, start_day, end_day, time_step_seconds, data)

def create_azel_cmd_telemetry_report(base_output_dir, tz, start_day, end_day, time_step_seconds, data):
  satnum = data.get('satellite',[])['number']
  ground_station = GroundStation.from_config(data.get('ground_station',[]))
  tlm_file = data.get('tlm_file',"")

  if (tlm_file is not None):
    cwd = os.getcwd() + "/"
    filename = cwd + tlm_file
    pngname = filename + ".png"
    process_file(filename, base_output_dir, pngname, tz, satnum, ground_station, data)

def process_file(filename, eltdir, pngname, tzone, satnum, gs, data):

  tle_dir = data.get('tle_dir', "")
  lyear = lmonth = lday = 0
  ax = generate_azelrange_plot()
  tle = SatelliteTle.from_config(data.get('satellite',[]))
  aer = AzElRangeReportGenerator(eltdir, gs, 1, tle, tzone, 0, time_step_seconds = 15)
  aer.generate_azelrange_plot_groundconstraints(ax)
  debug = Configuration.get_config_boolean(data.get('debug','false'))
  summary = Configuration.get_config_boolean(data.get('summary','false'))

  points = 0
  azels = []
  with open(filename, "r") as f:
    i = 0
    for line in f:
      year = line[0:4]
      month = line[5:7]
      day = line[8:10]
      hour = line[11:13]
      minute = line[14:16]
      second = line[17:19]
      if (debug):
        print("Line: %s contains year month day hour minute second: %s %s %s %s %s %s" % (line, year, month, day, hour, minute, second))
      if ((lyear != year) or (lmonth != month) or (lday != day)):
        date = "%s-%s-%s" % (lyear, lmonth, lday)
        if (len(azels) > 0):
          points = points + len(azels)
          generate_azelrange_subplot(date, ax, azels, i, data)# as the number of days progresses... i grows... and the color becomes closer to white
        azels = []
        i = i + 1

        elglob = tle_dir + "%s-%s-%s/*.tle" % (year, month, day)
        elfile = glob.glob(elglob)

        if (debug):
          print("New date found:  %s-%s-%s.  From glob %s found TLE file:  %s." % (year, month, day, elglob, elfile[0]))

        tle = SatelliteTle(satnum, tle_file=elfile[0])
        if (debug):
          print("TLE:")
          print(tle)
        orb = Orbital(str(tle.get_satellite_number()), \
                          line1=tle.get_line1(), \
                          line2=tle.get_line2())


      instant = datetime(int(year), int(month), int(day), int(hour), int(minute), int(second))
      temp = tzone.localize(instant).astimezone(UTC)
      time = datetime(temp.year, temp.month, temp.day, temp.hour, temp.minute, temp.second)
      (az, el) = orb.get_observer_look(time, gs.get_longitude(), gs.get_latitude(), gs.get_elevation_in_meters())
      azels.append((time, az, el))
      #print("%s%s, %s, %s-%s-%s %s:%s:%s, %d, %d" % (tle.get_epoch_year(), tle.get_epoch_day(), \
      #    time, year, month, day, hour, minute, second, el, az))

      lyear = year
      lmonth = month
      lday = day
    date = "%s-%s-%s" % (lyear, lmonth, lday)
    if (len(azels) > 0):
      points = points + len(azels)
      generate_azelrange_subplot(date, ax, azels, i, data)

    if (summary):
      print("There were %d points and %d days of telemetry represented." % (points, i))

  plt.figure(1)
  plt.savefig(pngname)
  #plt.close(fig)

def generate_azelrange_plot():
  # CAVEAT EMPTOR:  It was easier to work with the azimuth in radians (0 to 2pi) and the elevation in degrees (0 to 90)

  plt.rc('grid', color='#000000', linewidth=1, linestyle='-')
  plt.rc('xtick', labelsize=10)
  plt.rc('ytick', labelsize=10)

  # force square figure and square axes looks better for polar, IMO
  fig = plt.figure(figsize=(8, 8))
  ax = fig.add_axes([0.1, 0.1, 0.8, 0.8],
                    projection='polar')

  ax.set_theta_zero_location("N")
  ax.text(0, 103, "N", fontsize=10)
  ax.text(np.pi/2, 107, "E", fontsize=10)
  ax.text(np.pi, 105, "S", fontsize=10)
  ax.text(3*np.pi/2, 105, "W", fontsize=10)

  x = np.arange(0, 2*np.pi + 0.1, 0.1)
  ax.fill_between(x, 90, 90, color='#ffff00', alpha=0.5) # Min el: 90 - angle, plt has 0 at bullseye
  ax.fill_between(x, 0, 0, color='#ffff00', alpha=0.5)  # Keyhole: 90 - angle, plt has 0 at bullseye

  theta = np.arange(0, 2*np.pi + 0.1, 0.1)
  r = theta*0 + 90
  ax.plot(theta, r, color='#ff0000')

  labels = []
  for angle in range(0, 105, 15):
      labels.append(angle)
  labels.reverse()
  ticks = []
  for label in labels:
      ticks.append(90-label)

  ax.set_yticks(ticks)
  ax.set_yticklabels(map(str, labels))
  ax.legend()
  return ax

def generate_azelrange_subplot(day, ax, azels, number, data):
  space = 64
  offset = 100
  xarr = []
  yarr = []
  i = 0
  summary = Configuration.get_config_boolean(data.get('summary','false'))
  table = Configuration.get_config_boolean(data.get('table','false'))

  if (summary):
    print("There are %d points on %s" % (len(azels), day))

  for azel in azels:
      if (table):
        print("%s, %s, %s" % (azel[0], azel[1], azel[2]))
      theta = azel[1]*np.pi/180.0
      xarr.append(theta)
      r = 90.0 - azel[2]
      yarr.append(r)
      i = i + 1
  n = int(256*number/space) + offset
  clr = '#%2.2x%2.2x%2.2x' % (0, n, 0)
  #print(clr)
  ax.scatter(xarr, yarr, color=clr, lw=1, label='STF-1')


# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()

