$(document).ready(function() {
   updateWeatherSensorData();
   });


/* temperature */

var updateWeather_timeout;

function updateWeatherSensorData() {
    requestURL = "http://xxx.xxx.xxx.xxx"; // your ESP's IP Adress
    if ( typeof updateWeatherSensorData.timeout == 'undefined' ) {
        // It has not... perform the initialization
        updateWeatherSensorData.timeout = 0;
    }
    //Get Weather Sensor Value
    $.ajax({
        url: requestURL,
        error: function(error){
            if(updateWeatherSensorData.timeout++ <10) {
                setTimeout(updateWeatherSensorData, 12000);
            }
            else {
                updateWeatherSensorData.timeout = 0;
            }
        },
        success: function(thedata){
            $("#press").html(thedata.press + " mbar");
            $("#temp").html(thedata.temp + " °C");
            $("#hum").html(thedata.hum + " %");
            updateWeatherSensorData.timeout = 0;
        },
        timeout: 12000 // sets timeout to 12 seconds
    });
updateWeather_timeout = setTimeout("updateWeatherSensorData()", 360000);	
}
