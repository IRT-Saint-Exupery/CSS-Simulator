-- © Copyright European Space Agency, 2023
-- This file is subject to the terms and conditions defined in file 'LICENCE.md',
-- which is part of this source code package. No part of the package, 
-- including this file, may be copied, modified, propagated, or distributed except 
-- according to the terms contained in the file ‘LICENCE.md’.

local pus_table = {
    st1 = {
      name = "ST[01] request verification",
      sub_services = {
        sub1 = "successful acceptance verification report",
        sub2 = "failed acceptance verification report",
        sub3 = "successful start of execution verification report",
        sub4 = "failed start of execution verification report",
        sub5 = "successful progress of execution verification report",
        sub6 = "failed progress of execution verification report",
        sub7 = "successful completion of execution verification report",
        sub8 = "failed completion of execution verification report",
        sub10 = "failed routing verification report"
      }
    },
    st2 = {
      name = "ST[02] device access",
      sub_services = {
        sub1 = "distribute on/off device commands",
        sub2 = "distribute register load commands",
        sub4 = "distribute CPDU commands",
        sub5 = "distribute register dump commands",
        sub6 = "register dump report",
        sub7 = "distribute physical device commands",
        sub8 = "acquire data from physical devices",
        sub9 = "physical device data report",
        sub10 = "distribute logical device commands",
        sub11 = "acquire data from logical devices",
        sub12 = "logical device data report"
      }
    },
    st3 = {
      name = "ST[03] housekeeping",
      sub_services = {
        sub1 = "create a housekeeping parameter report structure",
        sub2 = "create a diagnostic parameter report structure",
        sub3 = "delete housekeeping parameter report structures",
        sub4 = "delete diagnostic parameter report structures",
        sub5 = "enable the periodic generation of housekeeping parameter reports",
        sub6 = "disable the periodic generation of housekeeping parameter reports",
        sub7 = "enable the periodic generation of diagnostic parameter reports",
        sub8 = "disable the periodic generation of diagnostic parameter reports",
        sub9 = "report housekeeping parameter report structures",
        sub10 = "housekeeping parameter report structure report",
        sub11 = "report diagnostic parameter report structures",
        sub12 = "diagnostic parameter report structure report",
        sub25 = "housekeeping parameter report",
        sub26 = "diagnostic parameter report",
        sub27 = "generate a one shot report for housekeeping parameter report structures",
        sub28 = "generate a one shot report for diagnostic parameter report structures",
        sub29 = "append parameters to a housekeeping parameter report structure",
        sub30 = "append parameters to a diagnostic parameter report structure",
        sub31 = "modify the collection interval of housekeeping parameter report structures",
        sub32 = "modify the collection interval of diagnostic parameter report structures",
        sub33 = "report the periodic generation properties of housekeeping parameter report structures",
        sub34 = "report the periodic generation properties of diagnostic parameter report structures",
        sub35 = "housekeeping parameter report periodic generation properties report",
        sub36 = "diagnostic parameter report periodic generation properties report",
        sub37 = "apply parameter functional reporting configurations",
        sub38 = "create a parameter functional reporting definition",
        sub39 = "delete parameter functional reporting definitions",
        sub40 = "report parameter functional reporting definitions",
        sub41 = "parameter functional reporting definition report",
        sub42 = "add parameter report definitions to a parameter functional reporting definition",
        sub43 = "remove parameter report definitions from a parameter functional reporting definition",
        sub44 = "modify the periodic generation properties of parameter report definitions of a parameter functional reporting definition"
      }
    },
    st4 = {
      name = "ST[04] parameter statistics reporting",
      sub_services = {
        sub1 = "report the parameter statistics",
        sub2 = "parameter statistics report",
        sub3 = "reset the parameter statistics",
        sub4 = "enable the periodic parameter statistics reporting",
        sub5 = "disable the periodic parameter statistics reporting",
        sub6 = "add or update parameter statistics definitions",
        sub7 = "delete parameter statistics definitions",
        sub8 = "report the parameter statistics definitions",
        sub9 = "parameter statistics definition report"
      }
    },
    st5 = {
      name = "ST[05] event reporting",
      sub_services = {
        sub1 = "informative event report",
        sub2 = "low severity anomaly report",
        sub3 = "medium severity anomaly report",
        sub4 = "High severity anomaly report",
        sub5 = "enable the report generation of event definitions",
        sub6 = "disable the report generation of event definitions",
        sub7 = "report the list of disabled event definitions",
        sub8 = "disabled event definitions list report"
      }
    },
    st6 = {
      name = "ST[06] memory management",
      sub_services = {
        sub1 = "load object memory data",
        sub2 = "load raw memory data areas",
        sub3 = "dump object memory data",
        sub4 = "dumped object memory data report",
        sub5 = "dump raw memory data",
        sub6 = "dumped raw memory data report",
        sub7 = "check object memory data",
        sub8 = "checked object memory data report",
        sub9 = "check raw memory data",
        sub10 = "checked raw memory data report",
        sub11 = "load a raw memory atomic data area in a non-interruptible transaction",
        sub12 = "abort all memory dumps",
        sub13 = "enable the scrubbing of a memory",
        sub14 = "disable the scrubbing of a memory",
        sub15 = "enable the write protection of a memory",
        sub16 = "disable the write protection of a memory",
        sub17 = "check an object memory object",
        sub18 = "checked object memory object report",
        sub19 = "load raw memory data areas by reference",
        sub20 = "dump raw memory data areas to file",
        sub21 = "load object memory data areas by reference",
        sub22 = "dump object memory data areas to file"
      }
    },
    st7 = {
      name = "ST[07] (reserved)"
    },
    st8 = {
      name = "ST[08] function management",
      sub_services = {
        sub1 = "perform a function"
      }
    },
    st9 = {
      name = "ST[09] time management",
      sub_services = {
        sub1 = "set the time report generation rate",
        sub2 = "CUC time report",
        sub3 = "CDS time report"
      }
    },
    st10 = {
      name = "ST[10] (reserved)"
    },
    st11 = {
      name = "ST[11] time-based scheduling",
      sub_services = {
        sub1 = "enable the time-based schedule execution function",
        sub2 = "disable the time-based schedule execution function",
        sub3 = "reset the time-based schedule",
        sub4 = "insert activities into the time-based schedule",
        sub5 = "delete time-based scheduled activities identified by request identifier",
        sub6 = "delete the time-based scheduled activities identified by a filter",
        sub7 = "time-shift scheduled activities identified by request identifier",
        sub8 = "time-shift the scheduled activities identified by a filter",
        sub9 = "detail-report time-based scheduled activities identified by request identifier",
        sub10 = "time-based schedule detail report",
        sub11 = "detail-report the time-based scheduled activities identified by a filter",
        sub12 = "summary-report time-based scheduled activities identified by request identifier",
        sub13 = "time-based schedule summary report",
        sub14 = "summary-report the time-based scheduled activities identified by a filter",
        sub15 = "time-shift all scheduled activities",
        sub16 = "detail-report all time-based scheduled activities",
        sub17 = "summary-report all time-based scheduled activities",
        sub18 = "report the status of each time-based sub-schedule",
        sub19 = "time-based sub-schedule status report",
        sub20 = "enable time-based sub-schedules",
        sub21 = "disable time-based sub-schedules",
        sub22 = "create time-based scheduling groups",
        sub23 = "delete time-based scheduling groups",
        sub24 = "enable time-based scheduling groups",
        sub25 = "disable time-based scheduling groups",
        sub26 = "report the status of each time-based scheduling group",
        sub27 = " time-based scheduling group status report"
      }
    },
    st12 = {
      name = "ST[12] on-board monitoring",
      sub_services = {
        sub1 = "enable parameter monitoring definitions",
        sub2 = "disable parameter monitoring definitions",
        sub3 = "change the maximum transition reporting delay",
        sub4 = "delete all parameter monitoring definitions",
        sub5 = "add parameter monitoring definitions",
        sub6 = "delete parameter monitoring definitions",
        sub7 = "modify parameter monitoring definitions",
        sub8 = "report parameter monitoring definitions",
        sub9 = "parameter monitoring definition report",
        sub10 = "report the out-of-limits",
        sub11 = "out-of-limits report",
        sub12 = "check transition report",
        sub13 = "report the status of each parameter monitoring definition",
        sub14 = "parameter monitoring definition status report",
        sub15 = "enable the parameter monitoring function",
        sub16 = "disable the parameter monitoring function",
        sub17 = "enable the functional monitoring function",
        sub18 = "disable the functional monitoring function",
        sub19 = "enable functional monitoring definitions",
        sub20 = "disable functional monitoring definitions",
        sub21 = "protect functional monitoring definitions",
        sub22 = "unprotect functional monitoring definitions",
        sub23 = "add functional monitoring definitions",
        sub24 = "delete functional monitoring definitions",
        sub25 = "report functional monitoring definitions",
        sub26 = "functional monitoring definition report",
        sub27 = "report the status of each functional monitoring definition",
        sub28 = "functional monitoring definition status report"
      }
    },
    st13 = {
      name = "ST[13] large packet transfer",
      sub_services = {
        sub1 = "first downlink part report",
        sub2 = "intermediate downlink part report",
        sub3 = "last downlink part report",
        sub9 = "uplink the first part",
        sub10 = "uplink an intermediate part",
        sub11 = "uplink the last part",
        sub16 = "large packet uplink abortion report"
      }
    },
    st14 = {
      name = "ST[14] real-time forwarding control",
      sub_services = {
        sub1 = "add report types to the application process forward-control configuration",
        sub2 = "delete report types from the application process forward-control configuration",
        sub3 = "report the content of the application process forward-control configuration",
        sub4 = "application process forward-control configuration content report",
        sub5 = "add structure identifiers to the housekeeping parameter report forward-control configuration",
        sub6 = "delete structure identifiers from the housekeeping parameter report forward-control configuration",
        sub7 = "report the content of the housekeeping parameter report forward-control configuration",
        sub8 = "housekeeping parameter report forward-control configuration content report",
        sub9 = "add structure identifiers to the diagnostic parameter report forward-control configuration",
        sub10 = "delete structure identifiers from the diagnostic parameter report forward-control configuration",
        sub11 = "report the content of the diagnostic parameter report forward-control configuration",
        sub12 = "diagnistic parameter report forward-control configuration content report",
        sub13 = "delete event definition identifiers from the event report blocking forward-control configuration",
        sub14 = "add event definition identifiers to the event report blocking forward-control configuration",
        sub15 = "report the content of the event report blocking forward-control configuration",
        sub16 = "event report blocking forward-control configuration content report"
      }
    },
    st15 = {
      name = "ST[15] on-board storage and retrieval",
      sub_services = {
        sub1 = "enable the storage function of packet stores",
        sub2 = "disable the storage function of packet stores",
        sub3 = "add report types to the application process storage-control configuration",
        sub4 = "delete report types from the application process storage-control configuration",
        sub5 = "report the content of the application process storage-control configuration",
        sub6 = "application process storage-control configuration content report",
        sub9 = "start the by-time-range retrieval of packet stores",
        sub11 = "delete the content of packet stores up to the specified time",
        sub12 = "summary-report the content of packet stores",
        sub13 = "packet store content summary report",
        sub14 = "change the open retrieval start time tag of packet stores",
        sub15 = "resume the open retrieval of packet stores",
        sub16 = "suspend the open retrieval of packet stores",
        sub17 = "abort the by-time-range retrieval of packet stores",
        sub18 = "report the status of each packet store",
        sub19 = "packet store status report",
        sub20 = "create packet stores",
        sub21 = "delete packet stores",
        sub22 = "report the configuration of each packet store",
        sub23 = "packet store configuration report",
        sub24 = "copy the packets contained in a packet store selected by time window",
        sub25 = "resize packet stores",
        sub26 = "change a packet store type to circular",
        sub27 = "change a packet store type to bounded",
        sub28 = "change the virtual channel used by a packet store",
        sub29 = "add structure identifiers to the housekeeping parameter report storage-control configuration",
        sub30 = " delete structure identifiers from the housekeeping parameter report storage-control configuration",
        sub31 = "add structure identifiers to the diagnostic parameter report storage-control configuration",
        sub32 = " delete structure identifiers from the diagnostic parameter report storage-control configuration",
        sub33 = "delete event definition identifiers from the event report blocking storage-control configuration",
        sub34 = "add event definition identifiers to the event report blocking storage-control configuration",
        sub35 = "report the content of the housekeeping parameter report storage-control configuration",
        sub36 = "housekeeping parameter report storage-control configuration content report",
        sub37 = "report the content of the diagnostic parameter report storage-control configuration",
        sub38 = "diagnostic parameter report storage-control configuration content report",
        sub39 = "report the content of the event report blocking storage-control configuration",
        sub40 = "event report blocking storage-control configuration content report"
      }
    },
    st16 = {
      name = "ST[16] (reserved)"
    },
    st17 = {
      name = "ST[17] test",
      sub_services = {
        sub1 = "perform an are-you-alive connection test",
        sub2 = "are-you-alive connection test report",
        sub3 = "perform an on-board connection test",
        sub4 = "on-board connection test report"
      }
    },
    st18 = {
      name = "ST[18] on-board control procedure",
      sub_services = {
        sub1 = "direct-load an OBCP",
        sub2 = "unload an OBCP",
        sub3 = "activate an OBCP",
        sub4 = "stop an OBCP",
        sub5 = "suspend an OBCP",
        sub6 = "resume an OBCP",
        sub7 = "communicate parameters to an OBCP",
        sub8 = "report the execution status of each OBCP",
        sub9 = "OBCP execution status report",
        sub12 = "abort an OBCP",
        sub13 = "load an OBCP by reference",
        sub14 = "activate and execute one OBCP step",
        sub15 = "resume and execute one OBCP step",
        sub16 = "set the observability level of OBCPs",
        sub17 = "abort all OBCPs and report",
        sub18 = "aborted OBCP report",
        sub19 = "load by reference and activate an OBCP",
        sub20 = "stop and unload an OBCP",
        sub21 = "start the OBCP engine",
        sub22 = "stop the OBCP engine"
      }
    },
    st19 = {
      name = "ST[19] event-action",
      sub_services = {
        sub1 = "add event-action definitions",
        sub2 = "delete event-action definitions",
        sub3 = "delete all event-action definitions",
        sub4 = "enable event-action definitions",
        sub5 = "disable event-action definitions",
        sub6 = "report the status of each event-action definition",
        sub7 = "event-action status report",
        sub8 = "enable the event-action function",
        sub9 = "disable the event-action function",
        sub10 = "report event-action definitions",
        sub11 = "event-action definition report"
      }
    },
    st20 = {
      name = "ST[20] on-board parameter management",
      sub_services = {
        sub1 = "report parameter values",
        sub2 = "parameter value report",
        sub3 = "set parameter values",
        sub4 = "change raw memory parameter definitions",
        sub5 = "change object memory parameter definitions",
        sub6 = "report parameter definitions",
        sub7 = "parameter definition report"
      }
    },
    st21 = {
      name = "ST[21] request sequencing",
      sub_services = {
        sub1 = "direct-load a request sequence",
        sub2 = "load a request sequence by reference",
        sub3 = "unload a request sequence",
        sub4 = "activate a request sequence",
        sub5 = "abort a request sequence",
        sub6 = "report the execution status of each request sequence",
        sub7 = "request sequence execution status report",
        sub8 = "load by reference and activate a request sequence",
        sub9 = "checksum a request sequence",
        sub10 = "request sequence checksum report",
        sub11 = "report the content of a request sequence",
        sub12 = "request sequence content report",
        sub13 = "abort all request sequences and report",
        sub14 = "aborted request sequence report"
      }
    },
    st22 = {
      name = "ST[22] position-based scheduling",
      sub_services = {
        sub1 = "enable the position-based schedule execution function",
        sub2 = "disable the position-based schedule execution function",
        sub3 = "reset the position-based schedule",
        sub4 = "insert activities into the position-based schedule",
        sub5 = "delete position-based scheduled activities identified by request identifier",
        sub6 = "delete the position-based scheduled activities identified by a filter",
        sub7 = "position-shift scheduled activities identified by request identifier",
        sub8 = "position-shift the scheduled activities identified by a filter",
        sub9 = "detail-report position-based scheduled activities identified by request identifier",
        sub10 = "position-based schedule detail report",
        sub11 = "detail-report the position-based scheduled activities identified by a filter",
        sub12 = "summary-report position-based scheduled activities identified by request identifier",
        sub13 = "position-based schedule summary report",
        sub14 = "summary-report the position-based scheduled activities identified by a filter",
        sub15 = "position-shift all scheduled activities",
        sub16 = "detail-report all position-based scheduled activities",
        sub17 = "summary-report all position-based scheduled activities",
        sub18 = "report the status of each position-based sub-schedule",
        sub19 = "position-based sub-schedule status report",
        sub20 = "enable position-based sub-schedules",
        sub21 = "disable position-based sub-schedules",
        sub22 = "create position-based scheduling groups",
        sub23 = "delete position-based scheduling groups",
        sub24 = "enable position-based scheduling groups",
        sub25 = "disable position-based scheduling groups",
        sub26 = "report the status of each position-based scheduling group",
        sub27 = "position-based scheduling group status report",
        sub28 = "set the orbit number"
      }
    },
    st23 = {
      name = "ST[23] file management",
      sub_services = {
        sub1 = "create a file",
        sub2 = "delete a file",
        sub3 = "report the attributes of a file",
        sub4 = "file attribute report",
        sub5 = "lock a file",
        sub6 = "unlock a file",
        sub7 = "find files",
        sub8 = "found files report",
        sub9 = "create a directory",
        sub10 = "delete a directory",
        sub11 = "rename a directory",
        sub12 = "summary-report the content of a repository",
        sub13 = "repository content summary report",
        sub14 = "copy a file",
        sub15 = "move a file",
        sub16 = "suspend file copy operations",
        sub17 = "resume file copy operations",
        sub18 = "abort file copy operations",
        sub19 = "suspend all file copy operations involving a repository path",
        sub20 = "resume all file copy operations involving a repository path",
        sub21 = "abort all file copy operations involving a repository path",
        sub22 = "enable the periodic reporting of the file copy status",
        sub23 = "file copy status report",
        sub24 = "disable the periodic reporting of the file copy status"
      }
    }
  }

return pus_table