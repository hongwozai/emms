<html>
<body>
<?php 
session_start();
$uname=$_GET['post'];
//echo $uname;
include("../conn.php");
//$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
$stmt = oci_parse($conn, "update T_USER set VERIFY='1' where USERNAME='$uname'");
oci_execute($stmt);
if(!oci_fetch($stmt)){
?>
   <script language="JavaScript">
     alert("同意用户注册！");
     window.history.go(-1);
    </script>  
    <?php }
    else{
    ?>
    <script language="JavaScript">
     alert("失败操作！");
     window.history.go(-1);
    </script> 
    <?php }?>
</body>
</html>
