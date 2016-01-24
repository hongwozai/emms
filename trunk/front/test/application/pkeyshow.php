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
			<h4>封堵关键字列表显示</h4>
		</ul>
		
		<form name="myform" action="../application/pkeyshow.php" method="post">
		<label for="username" class="label">类型选择：</label>
		<select name="number">
		 
     <option value ="0">发件人</option>  
     <option value ="1">收件人</option>  
     <option value="2">主题</option>  
     <option value="3">内容</option> 
     <option value="4">附件名</option>
     <option value="5">附件内容</option> 

		</select>&nbsp;&nbsp;&nbsp;&nbsp;
		<button type="submit" name="submit">确定</button>
		<br></br>

    <table class="table" border="1px" style="border:1px solid #ddd;" align="center" id="mytable">
		<caption class="text-center"><h4>封堵关键字列表显示</h4></caption>
		<tr>
		<th style="border:1px solid #ddd;height:30px;" align="center" bgcolor="#f1f1f1">关键字</th>
		<th style="border:1px solid #ddd;height:30px;" align="center" bgcolor="#f1f1f1">操作类型</th>
		</tr>
<?php 
      session_start();
      $t1=$_POST['number'];
     // echo $t1;
      $t2=$_GET['number'];
     // echo $t2;
      if($t1==NULL) $type=$t2;
      if($t2==null) $type=$t1;
      if($_SESSION['username']){
          include("../conn.php");
       // $conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
       /* $stmt = oci_parse($conn, "select * from KILL_KEY where TYPE='$type'");
        oci_define_by_name($stmt, "KEYWORD", $keyword);
        oci_define_by_name($stmt, "TYPE", $type);
        oci_define_by_name($stmt, "KILL_ID", $id);
        oci_execute($stmt);
      while(oci_fetch($stmt)){*/
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
          $sql_exc_page = OCIParse($conn,"select * from KILL_KEY where TYPE='$type' order by KILL_ID desc");
          OCIExecute($sql_exc_page);
          $toltalnum = oci_fetch_all($sql_exc_page,$result);
          $totalpage = ceil($toltalnum/$perNumber);
          $sql_fenye = "select * from KILL_KEY where TYPE='$type' order by KILL_ID desc";
          $sql_exc_fenye = oci_parse($conn, $sql_fenye);
          oci_execute($sql_exc_fenye);
          $mID = 0;
           
          for($i = 0; $i <= $perNumber * ($page - 1); $i++)
          {
              if( ( $row = oci_fetch_assoc($sql_exc_fenye) ) != false)
              {
                  $mID = $row['KILL_ID'];
              }
              else
              {
                  print "<script>alert('无记录')</script>";
              }
          }
          $sql_fenye = "select * from KILL_KEY where TYPE='$type' and KILL_ID <= $mID  order by KILL_ID desc";
          $sql_exc = oci_parse($conn, $sql_fenye);
          oci_execute($sql_exc);
          for ($i = 0; $i < $perNumber; $i++){
              if(($row = oci_fetch_assoc($sql_exc)) != false){
                          if($row['TYPE']==0){ ?>
             <tr>
              <td style="border:1px solid #ddd;" width=100px><?php echo $row['KEYWORD'];?></td>
              <td style="border:1px solid #ddd;" width=100px><a href="../application/delp.php?postid=<?php echo $row['KILL_ID'];?>&numb=<?php echo $row['TYPE'];?>">删除</a></td>
             </tr>
         <?php } 
      if($row['TYPE']==1){ ?> 
             <tr>
             
            <td style="border:1px solid #ddd;" width=100px><?php echo $row['KEYWORD'];?></td>
              <td style="border:1px solid #ddd;" width=100px><a href="../application/delp.php?postid=<?php echo $row['KILL_ID'];?>&numb=<?php echo $row['TYPE'];?>">删除</a></td>
             </tr>
       <?php    }
      if($row['TYPE']==2){ ?>
            <tr>
            
            <td style="border:1px solid #ddd;" width=100px><?php echo $row['KEYWORD'];?></td>
              <td style="border:1px solid #ddd;" width=100px><a href="../application/delp.php?postid=<?php echo $row['KILL_ID'];?>&numb=<?php echo $row['TYPE'];?>">删除</a></td>
             </tr>
        <?php   }
          if($row['TYPE']==3){ ?>
              <tr>
             
            <td style="border:1px solid #ddd;" width=100px><?php echo $row['KEYWORD'];?></td>
              <td style="border:1px solid #ddd;" width=100px><a href="../application/delp.php?postid=<?php echo $row['KILL_ID'];?>&numb=<?php echo $row['TYPE'];?>">删除</a></td>
             </tr>
         <?php  }
          if($row['TYPE']==4){ ?>
             <tr>
             
            <td style="border:1px solid #ddd;" width=100px><?php echo $row['KEYWORD'];?></td>
              <td style="border:1px solid #ddd;" width=100px><a href="../application/delp.php?postid=<?php echo $row['KILL_ID'];?>&numb=<?php echo $row['TYPE'];?>">删除</a></td>
             </tr>
        <?php  }
          if($row['TYPE']==5){ ?>
             <tr>
             
            <td style="border:1px solid #ddd;" width=100px><?php echo $row['KEYWORD'];?></td>
              <td style="border:1px solid #ddd;" width=100px><a href="../application/delp.php?postid=<?php echo $row['KILL_ID'];?>&numb=<?php echo $row['TYPE'];?>">删除</a></td>
             </tr>
         <?php    
          }
        }
      }
      }
?>
		
    </table>
    </form>
    <li align="right">
        <a href="../application/pkeyshow.php?page=<?php echo 1;?>&number=<?php echo $type;?>" target="_self">首页</a>
        <?php 
        if($page != 1)		
        {
        ?>
        <a href="../application/pkeyshow.php?page=<?php echo $page - 1;?>&number=<?php echo $type;?>" target="_self">上页 </a>
        <?php  
        }
        if ($page < $totalpage )
        {
        ?>
        <a href="../application/pkeyshow.php?page=<?php echo $page +1; ?>&number=<?php echo $type;?>" target="_self">下页</a>
        <?php	
        }
        ?>
        <a href="../application/pkeyshow.php?page=<?php echo $totalpage;?>&number=<?php echo $type;?>" target="_self">尾页 </a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</li>          
	
    </div>
</body>
</html>
