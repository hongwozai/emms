<!DOCTYPE html>
<html>
<head>
<admintpl file="header"/>
<style>
.wrap {
padding: 20px 20px 70px;
}
.nav-tabs {
border-bottom: 1px solid #ddd;
}
.table-bordered {
border: 1px solid #ddd;
border-collapse: separate;
border-left: 0;
-webkit-border-radius: 3px;
-moz-border-radius: 3px;
border-radius: 3px;
}
.table {
width: 60%;
margin-bottom: 20px;
}
table {
max-width: 60%;
background-color: transparent;
border-collapse: collapse;
border-spacing: 0;
}

</style>
</head>
<body>
<div class="wrap J_check_wrap">
		<ul class="nav nav-tabs">
			<h4>注册用户</h4>
		</ul>
		<form name="myform" action="" method="post">
		 <table class="table" style="border:1px solid #ddd;" align="center">
            <caption class="text-center"><h4>注册用户信息表</h4></caption>
        <tr>
        <th style="border:1px solid #ddd;" align="center" bgcolor="#f1f1f1">用户名</th>
        <th style="border:1px solid #ddd;" align="center" bgcolor="#f1f1f1">申请权限</th>
        <th style="border:1px solid #ddd;" align="center" bgcolor="#f1f1f1">操作</th>
      </tr>
      <?php 
      session_start();
      if($_SESSION['priority']==0){
     if($_SESSION['username']){
         include("../conn.php");
       // $conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
        $stmt = oci_parse($conn, "select * from T_USER where VERIFY='0'");
        oci_define_by_name($stmt, "USERNAME", $username);
        oci_define_by_name($stmt, "PRIORITY", $priority);
        oci_execute($stmt);
      while(oci_fetch($stmt)){?>
      <tr>
            <td style="border:1px solid #ddd;" align="center"><?php echo $username;?></td>
            <td style="border:1px solid #ddd;" align="center"><?php echo $priority;?></td>
            <td style="border:1px solid #ddd;" align="center"><a href="../application/modverify.php?post=<?php echo $username;?>" type="submit">同意</a>
            |<a href="../application/refuse.php?post=<?php echo $username;?>" type="submit">拒绝</a></td>
        </tr>     
     <?php  }   
      }
      } else{
        echo "<script language=\"JavaScript\">\r\n";
        echo "alert(\"您的权限不够，谢谢!\");\r\n";
        echo "history.back();\r\n";
        echo "</script>";
    }?>
      </table>
		</form>
	</div>
</body>
</html>