<html>

<body>

<?php
  session_start();
  if(isset($_SESSION['username'])){
      $username=$_SESSION['username'];
  $p=$_POST['old_password'];
  $p1=$_POST['password'];
  $p2=$_POST['repassword'];
  if($p1==$p2){
      include("../conn.php");
  //$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
  if($conn){
  $stmt = oci_parse($conn, "update T_USER set PASSWORD='$p1' where USERNAME='$username'");
  $e=oci_execute($stmt);
  if($e=TRUE){
      oci_commit($conn);
      echo "<script language=\"JavaScript\">\r\n";
     echo "alert(\"更新成功！\");\r\n";
     echo "history.back();\r\n";
     echo "</script>";     
  }
  }
  }
  else{
      echo "<script language=\"JavaScript\">\r\n";
      echo "alert(\"两次输入的密码不一样！\");\r\n";
      echo "history.back();\r\n";
      echo "</script>";
  }
  }
  else{
      ECHO "CHIBAI";
      header("Location:http://127.0.0.1/www/test/error.php");
  }

?>
</body>
</html>
