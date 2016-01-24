<!DOCTYPE html>
<html>
<head>
<script language="javascript">
window.onload=function showtable(){
var tablename=document.getElementById("mytable");
var li=tablename.getElementsByTagName("tr");
for (var i=0;i<=li.length;i++){
if (i%2==0){
li[i].style.backgroundColor="#f1f1f1";
}else li[i].style.backgroundColor="#FFFFFF";
}
}
</script>
<style>
.wrap {
padding: 20px 20px 70px;

}
.nav-tabs {
border-bottom: 1px solid #ddd;
}
.table {
width: 80%;
margin-bottom: 20px;
}
table {
max-width: 80%;
background-color: transparent;
border-collapse: collapse;
border-spacing: 0;
}
#btn_wraper{
				text-align: center;
			}
.table{
				text-align: center;
			}
</style>

</head>
<body>
<div class="wrap">
		<ul class="nav-tabs">
			<h4>用户列表显示</h4>
		</ul>
		
		<form name="myform" action="" method="post">
		 <table class="table" style="border:1px solid #ddd;" align="center" id="mytable">
       <caption class="text-center"><h4>用户信息表</h4></caption>
        <tr>
        <th style="border:1px solid #ddd;height:30px;" align="center" bgcolor="#f1f1f1">用户名</th>
        <th style="border:1px solid #ddd;height:30px;" align="center" bgcolor="#f1f1f1">申请权限</th>
        <th style="border:1px solid #ddd;height:30px;" align="center" bgcolor="#f1f1f1">操作</th>
      </tr>
<?php 
      session_start();
      $u=$_SESSION['username'];
      if($_SESSION['priority']==0){
     if($_SESSION['username']){
         include("../conn.php");
        //$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
        $stmt = oci_parse($conn, "select * from T_USER where USERNAME<>'$u'");
        oci_define_by_name($stmt, "USERNAME", $username);
        oci_define_by_name($stmt, "PRIORITY", $priority);
        oci_execute($stmt);
      while(oci_fetch($stmt)){
       ?>
      <script text="text/javascript">
function getSelect(name){
	var node = document.getElementById("select" + name);
	var str = "../application/modpriority.php?postid="+name +"&value=" + node.value;
	location.href=str;
}
</script>
           <tr>
            <td style="border:1px solid #ddd;" align="center"><?php echo $username;?></td>
          <td style="border:1px solid #ddd;" align="center"><?php 
          if($priority==0) echo "超级管理员";
            else if($priority==1) echo "管理员";
               else echo "普通用户";          
          ?></td>
            <td style="border:1px solid #ddd;" width=200px align="center">
            <select id="<?php echo 'select'.$username;?>" style="border:1px solid #ddd;">
                <option value ="2">2普通用户</option> 
                <option value ="1">1管理员</option>
                <option value ="0">0超级管理员</option>
               </select><a id="<?php echo $username?>" href="javascript:void(0)" onclick="getSelect('<?php echo $username?>')">修改权限</a></td>
           </tr>
         <?php  }
        
}

      }
      
      else{?>
          <script language="JavaScript">
         alert("您的权限不够，无法操作该页面，谢谢!");
         history.back(-1);
         </script>
     <?php  }?>

</table>
		</form>
		
	</div>

</body>
</html>
