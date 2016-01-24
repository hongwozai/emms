<html>
<body>
<?php 
$uname=$_GET['post'];
//echo $uname;
include("../conn.php");
//$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
$stmt = oci_parse($conn, "delete from T_USER where USERNAME='$uname'");
oci_execute($stmt);
if(!oci_fetch($stmt)){
?>
    <script language="JavaScript">
     alert("成功删除该注册用户！");
     window.history.go(-1);
    </script> 
    <?php }
    else{
    ?>
    <script language="JavaScript">
     alert("删除该注册用户,失败！");
     window.history.go(-1);
    </script>
    <?php }?>
</body>
</html>
