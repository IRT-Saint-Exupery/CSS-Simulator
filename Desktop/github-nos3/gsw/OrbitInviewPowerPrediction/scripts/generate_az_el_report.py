#!/usr/bin/env python
import sys
import json
import tempfile
from pytz import timezone
from configuration import Configuration
from satellite_tle import SatelliteTle
from ground_station import GroundStation
from satellite_html_report_generator import SatelliteHtmlReportGenerator
from ground_station_html_report_generator import GroundStationHtmlReportGenerator
from angular_separation_report import AngularSeparationReportGenerator
from az_el_range_report import  AzElRangeReportGenerator
from satellite_overflight_report_generator import SatelliteOverflightReportGenerator
from inview_list_report_generator import InviewListReportGenerator
from inview_calculator import InviewCalculator
from datetime import datetime, timedelta

def main():
    """Main function... makes 'forward declarations' of helper functions unnecessary"""
    conf_file = "config/sat_html_report.config"
    if (len(sys.argv) > 1):
        conf_file = sys.argv[1]
    #sys.stderr.write("conf_file: %s\n" % conf_file)
    
    with open(conf_file) as json_data_file:
        data = json.load(json_data_file)
        
    #base_output_dir = Configuration.get_config_directory(data.get('base_output_directory',tempfile.gettempdir()))
    tz = Configuration.get_config_timezone(data.get('timezone','US/Eastern'))
    inviews = Configuration.get_config_boolean(data.get('inviews','false'))
    contacts = Configuration.get_config_boolean(data.get('contacts','false'))
    insun = Configuration.get_config_boolean(data.get('insun','false'))
    start_day = Configuration.get_config_int(data.get('start_day','0'), -180, 180, 0)
    end_day = Configuration.get_config_int(data.get('end_day','0'), -180, 180, 0)
    time_step_seconds = Configuration.get_config_float(data.get('time_step_seconds','15'), 1, 600, 15)
    sat_tle = SatelliteTle.from_config(data.get('satellite',[]))
    ground_station = GroundStation.from_config(data.get('ground_station',[]))    
    
    # Determine the time range for the requested day
    day_date = datetime.now() + timedelta(days=start_day)
    day_year = day_date.year
    day_month = day_date.month
    day_day = day_date.day
    start_time = tz.localize(
        datetime(day_year, day_month, day_day, 0, 0, 0))
    end_time = tz.localize(
        datetime(day_year, day_month, day_day, 23, 59, 59))

    # Get the InviewCalculator and compute the inviews
    base_ic = InviewCalculator(ground_station, sat_tle)
    base_inviews = []
    base_inviews = base_ic.compute_inviews(start_time, end_time)
    for iv in base_inviews:
        print(iv) # debug
        start_time = iv[0].astimezone(tz)
        end_time = iv[1].astimezone(tz)
        azels = base_ic.compute_azels(iv[0], iv[1], time_step_seconds)
        for azel in azels:
            #print("      %s, %7.2f,    %6.2f,   %7.1f\n" % (azel[0].astimezone(tz), azel[1], azel[2], azel[3])) # convert to specified time zone
            print("      %s, %7.2f,    %6.2f,   %7.1f" % (azel[0].astimezone(tz), azel[1], azel[2], azel[3])) # convert to specified time zone
            
    #print(sat_tle)
    
   
# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()
