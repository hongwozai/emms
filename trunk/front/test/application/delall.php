<html>
<body>
<?php 
session_start();
if($_SESSION['priority']==2){?>
                     <script type="text/javascript">
                        alert("您没有权限操作！");
                        window.history.go(-1);
                        </script>
                    <?php }else{
$name=$_GET['name'];
$pro=$_GET['value'];
include("../conn.php");
//$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
$stmt = oci_parse($conn, "delete from $name where PROTOCOL='$pro'");
$e=oci_execute($stmt);
oci_commit($conn);
if($e==true){?>
    <script language="JavaScript">
    alert("delete success");
    window.location.href="../application/log.php?lx=<?php echo $name;?>&xy=<?php echo $pro;?>";
   </script>
    
<?php }

                    }
?>
</body>
</html>