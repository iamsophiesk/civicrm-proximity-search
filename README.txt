

# CiviCRM Proximity

CiviCRM Proximity enables geographical proximity search for the CiviCRM 
module.


# Views integration

OpenLayers Proximity exposes:

 * Square filter: it gives locations contained within a square derived by a 
   simple latitude/longitude comparison. Less accurate, better performance
 * Great-circle filter: it uses the Great-circle distance formula to return 
   locations within a circular area. More accurate, lower performance.



# Configuration 

Before enabling the module it is necessary to enable geocoding in your CiviCRM
database and run the script that populates those fields.

 http://wiki.civicrm.org/confluence/display/CRMDOC33/Batch+Geocoding+Script