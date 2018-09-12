<?php
$handle = curl_init();

//Select request method and url
curl_setopt($handle, CURLOPT_URL, "http://cloud.iot-playground.com:40404/RestApi/v1.0/Module/List");
curl_setopt($handle, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($handle, CURLOPT_CUSTOMREQUEST, "GET");

//Passing Auth header
$auth = array();
$auth[] = "Eiot-Authtoken: 1Zb2nzPiaG4G0MpahtwZqPzU9iY9wQiiglWuUHbo";
curl_setopt($handle, CURLOPT_HTTPHEADER, $auth);

//saving response and displaying any errors
$response = curl_exec($handle);
if (curl_errno($handle)) {
    echo 'Error:' . curl_error($handle);
}
//close connection 
curl_close ($handle);
echo $response;
?>