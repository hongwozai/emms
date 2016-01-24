<html>
<body>
<?php 
$uname=$_GET['postid'];
$priority=$_GET['value'];
//echo $uname,$priority;
session_start();
//echo $_SESSION['priority'];
if($_SESSION['priority']==0){
    include("../conn.php");
   // $conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
    $stmt = oci_parse($conn, "update T_USER set PRIORITY='$priority' where USERNAME='$uname'");
    $e=oci_execute($stmt);
  if($e=TRUE){
      oci_commit($conn);
      echo "<script language=\"JavaScript\">\r\n";
     echo "alert(\"修改用户权限成功！\");\r\n";
     echo "history.back();\r\n";
     echo "</script>";     
  }
}
else{
    echo "<script language=\"JavaScript\">\r\n";
    echo "alert(\"您的权限不够，谢谢!\");\r\n";
    echo "history.back();\r\n";
    echo "</script>";
}



?>
</body>
</html>
