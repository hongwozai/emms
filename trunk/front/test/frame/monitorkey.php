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
.white { 
color: #606060; 
border: solid 1px #b7b7b7; 
background: #fff; 
background: -webkit-gradient(linear, left top, left bottom, from(#fff), to(#ededed)); 
background: -moz-linear-gradient(top, #fff, #ededed); 
filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#ffffff', endColorstr='#ededed'); 
	width:60px;height:20px;
} 
</style>
<script type="text/javascript">
function add()
{
form.action="../application/insertmkey.php"; 
}
function reset()
{
 form.action="monitorkey.php";
}
</script>
</head>
<body>
<div class="wrap J_check_wrap">
		<ul class="nav nav-tabs">
			<h4>添加监测关键字</h4>
		</ul>
		
		<form  method="post" name="form">
		<table>
		<td>
		&nbsp;&nbsp;&nbsp;&nbsp;<tr>
		<select name="number">
		 
     <option value ="0">发件人</option>  
     <option value ="1">收件人</option>  
     <option value="2">主题</option>  
     <option value="3">内容</option> 
     <option value="4">附件名</option>
     <option value="5">附件内容</option> 

		</select>&nbsp;&nbsp;&nbsp;&nbsp;</tr>
		<tr>
		<input class="input" id="mkey" required name="mkey" type="text" placeholder="监测关键字内容" title="监测关键字内容" value=""/>		
		</tr>
        </td>
        <td>
         <input type="submit" name="submit" class="button white" onclick="add()" value="添加">&nbsp;&nbsp;&nbsp;&nbsp;
        
         <input onclick="reset()" class="button white" type="submit" value="重置" />
        </td>
        </table>
        
    </form>
        <table class="table" style="border:1px solid #ddd;" align="center" id="mytable">
		<caption class="text-center"><h4>监测关键字列表显示</h4></caption>
		<tr>
		<th align="center" style="border:1px solid #ddd;" bgcolor="#f1f1f1" height="30px">类型</th>
		<th align="center" style="border:1px solid #ddd;"  bgcolor="#f1f1f1" height="30px">关键字</th>
		</tr>
		<?php 
		 include '../conn.php';
		 $perNumber = 8;
         $page = 1;
         
         if(isset($_GET['page']))
         {
             $page = $_GET['page'];
         }
         //$page = $_GET['page'];
         
         if( $page == false )
         {
             $page = 1;
         }
         $sql_exc_page = OCIParse($conn,"select * from MONITOR_KEY order by MONITOR_ID desc");
         OCIExecute($sql_exc_page);
         $toltalnum = oci_fetch_all($sql_exc_page,$result);
         $totalpage = ceil($toltalnum/$perNumber);
         $sql_fenye = "select * from MONITOR_KEY order by MONITOR_ID desc";
         $sql_exc_fenye = oci_parse($conn, $sql_fenye);
         oci_execute($sql_exc_fenye);
         $mID = 0;
         
         for($i = 0; $i <= $perNumber * ($page - 1); $i++)
         {
             if( ( $row = oci_fetch_assoc($sql_exc_fenye) ) != false)
             {
                 $mID = $row['MONITOR_ID'];
             }
             else
             {
                 print "<script>alert('无记录')</script>";
             }
         }
         $sql_fenye = "select * from MONITOR_KEY  where MONITOR_ID <= $mID  order by MONITOR_ID desc";
         $sql_exc = oci_parse($conn, $sql_fenye);
         oci_execute($sql_exc);
         for ($i = 0; $i < $perNumber; $i++){
              if(($row = oci_fetch_assoc($sql_exc)) != false){
		     ?>
		                  <tr>
		              <td style="border:1px solid #ddd;" width=100px align="center"><?php if($row['TYPE']==0) echo "发件人";
		              if($row['TYPE']==1) echo "收件人";
		              if($row['TYPE']==2) echo "主题";
		              if($row['TYPE']==3) echo "内容";
		              if($row['TYPE']==4) echo "附件名";
		              if($row['TYPE']==5) echo "附件内容";?></td>
		                 <td style="border:1px solid #ddd;" width=100px align="center"><?php echo $row['KEYWORD'];?></td>
		                 </tr>
		                  
		              <?php } 
         }
		?>
		</table>
		 <li align="right">
        <a href="../frame/monitorkey.php?page=<?php echo 1;?>" target="_self">首页</a>
        <?php 
        if($page != 1)		
        {
        ?>
        <a href="../frame/monitorkey.php?page=<?php echo $page - 1;?>" target="_self">上页 </a>
        <?php  
        }
        if ($page < $totalpage )
        {
        ?>
        <a href="../frame/monitorkey.php?page=<?php echo $page +1; ?>" target="_self">下页</a>
        <?php	
        }
        ?>
        <a href="../frame/monitorkey.php?page=<?php echo $totalpage;?>" target="_self">尾页 </a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</li>          
	</div>			
			
</body>
</html>
