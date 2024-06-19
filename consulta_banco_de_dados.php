<?php
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
// CODIGO PHP PARA CONEXÃO COM BANCO DE DADOS MySQL (CONSULTA ÚLTIMA MEDIDA DE LUX)
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

$servername 	= "localhost";	// SERVIDOR LOCAL RPi 
$username 		= "pi";			// USUARIO BANCO DE DADOS 
$password 		= "raspberry";	// SENHA BANCO DE DADOS
$dbname 		= "luminarias";	// BANCO DE DADOS
$dbTableLux		= "fluxo_luminoso"; // TABLE RECEIVED LUX 
$chipID_esp32	= "a6da08";	// ACTUAL ESP32 ID
$dataQuantity	= 1;		// QUANTIDADE DE MEDIDAS A RECEBER DO DB
// Criar conexão
$conn = new mysqli($servername, $username, $password, $dbname);// Verificar a conexão
if ($conn->connect_error) {
  die("Erro de conexão: " . $conn->connect_error);}
// Consulta SQL para obter os dados do banco de dados
// $sql = "SELECT * FROM sua_tabela";

$sql = "SELECT `lux` FROM $dbTableLux WHERE chipID = '$chipID_esp32' ORDER BY `id` DESC LIMIT $dataQuantity;";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
// Saída dos dados de cada linha
  while($row = $result->fetch_assoc()) {

    echo $row["lux"];
  }
} else {
  echo "0 resultados";
}
$conn->close();
?>
