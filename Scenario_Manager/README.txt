TO use simply launch the Terminator (double-click on Desktop icon) and use:

python3 manager.py scenarios/SC001.yaml  


Output directory structure in /tmp (see VM MCS):
  
  {output_dir}/
    css-pcap-ds/
      data/
        {split}/
          {mission_id}_ground_station.pcap
    css-logs-ds/
      data/
        {probe_id}/
          {split}/
            logids_sat{sat_id}.{csv}
            
           
