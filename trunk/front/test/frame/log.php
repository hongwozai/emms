<!DOCTYPE html>
<html>
<meta charset="UTF-8" />
<head>

<style>
.nav-tabs {
border-bottom: 1px solid #ddd;
}
.white { 
color: #606060; 
border: solid 1px #b7b7b7; 
background: #fff; 
background: -webkit-gradient(linear, left top, left bottom, from(#fff), to(#ededed)); 
background: -moz-linear-gradient(top, #fff, #ededed); 
filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#ffffff', endColorstr='#ededed'); 
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
width: 80%;
margin-bottom: 20px;
}
table {
max-width: 80%;
background-color: transparent;
border-collapse: collapse;
border-spacing: 0;
}

.button, .button:visited {
	background: #66CDAA url(overlay.png) repeat-x;
	display: inline-block;
	padding: 5px 10px 6px;
	color: #fff;
	text-decoration: none;
	-moz-border-radius: 6px;
	-webkit-border-radius: 6px;
	-moz-box-shadow: 0 1px 3px rgba(0,0,0,0.6);
	-webkit-box-shadow: 0 1px 3px rgba(0,0,0,0.6);
	text-shadow: 0 -1px 1px rgba(0,0,0,0.25);
	border-bottom: 1px solid rgba(0,0,0,0.25);
	position: relative;
	cursor: pointer
}
</style>

</head>
<body>
<?php error_reporting(0);?>
<div class=" ">
		<ul class="nav nav-tabs">
			<h4>日志管理</h4>
		</ul>
			 <form name="myform" action="../application/log.php" method="post"> 
		<table  align="center" valign="middle" border="0px">
		<thread>
		<tr>
			<th align="left">开始日期：			
				<input class="input" onclick="fPopCalendar(begin,begin);" return flase type="text" name="begin" size="30"/>
			</th>
			<th align="left">结束日期:			
				<input class="input" onclick="fPopCalendar(end,end);" return flase type="text" name="end" size="30"/>
			</th>
		</tr>
		<tr>
		  <th align="left">类型：
		  <input type="radio" name="lx" value="MONITOR_LOG"<?php if($_POST['lx'] == "MONITOR_LOG") ?> />监测
		  <input type="radio" name="lx" value="KILL_LOG"<?php if($_POST['lx'] == "KILL_LOG") ?> />封堵
		  <input type="radio" name="lx" value="SPAM_LOG"<?php if($_POST['lx'] == "SPAM_LOG" )?> />垃圾邮件
		  </th>
		</tr>
		<tr>
		  <th align="left">协议：
		  <input type="radio" name="xy" value="25" <?php if($_POST['xy'] == "25") ?> /> SMTP
		  <input type="radio" name="xy" value="110"<?php if($_POST['xy'] == "110") ?> /> POP3
		  <input type="radio" name="xy" value="143"<?php if($_POST['xy'] == "143") ?> /> IMAP
		  <input type="radio" name="xy" value="80"<?php if($_POST['xy'] == "80") ?> /> WEBMAIL
		  </th>
		  
		</tr>
		</thread>
		</table>
		<br>
		<table style="table-layout: fixed;" align="center">
		<tr>
		<td style="vertical-align:top;"height="10px">
		<div align="center">
		<form name="myform" action="../application/log.php" method="post">
		<button type="submit" name="submit" class="button" >查询</button>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
		</form></div>
		</td>
		<td style="vertical-align:top;"height="10px">
		<div align="center">
		<form name="form" action="../application/leadin.php" method="post" enctype="multipart/form-data">
		<button type="submit" name="submit" class="button" >导入</button>
	    <input type="file" name="file"> 
	    </form></div>
	    </td>
	    </tr>
	    </table>
	    </form>
		</div>
</body>
</html>
