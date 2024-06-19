<?php
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// CODIGO PHP PARA CONEXÃO COM BANCO DE DADOS MySQL (INSERÇÃO MEDIDAS DE LUX)
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

$servername = "localhost";
// REPLACE with your Database name
$dbname = "luminarias";
// REPLACE with Database user
$username = "pi";
// REPLACE with Database user password
$password = "raspberry";
// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "tPmAT5Ab3j7F9ContiCelesc";
$api_key = $lux = $chipID = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $lux = test_input($_POST["lux"]);
        $chipID = test_input($_POST["chipID"]);
// Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
        if ($conn -> connect_error) {
            die("Connection failed: " . $conn -> connect_error);
        } 
        $sql = "INSERT INTO fluxo_luminoso (lux, chipID)
        VALUES ('" . $lux . "', '" . $chipID . "')";
        if ($conn -> query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn -> error;
        }
        $conn -> close();
    }
    else {
        echo "Wrong API Key provided.";
    }
}
else {
    echo "BANCO DE DADOS - CLOVIS E FRANCISCO!!!";
}
function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>