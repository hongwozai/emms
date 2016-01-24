<!doctype html>

<html>
<head>
		<meta charset="UTF-8" />
		<title>EMMS电子邮件内容监控系统</title>
		<meta http-equiv="X-UA-Compatible" content="chrome=1,IE=edge" />
		<meta name="renderer" content="webkit|ie-comp|ie-stand">
		<meta name="robots" content="noindex,nofollow">
		<link href="css/login.css" rel="stylesheet" /> 
		<style>
			#login_btn_wraper{
				text-align: center;
			}
			#login_btn_wraper .tips_success{
				color:#fff;
			}
			#login_btn_wraper .tips_error{
				color:#DFC05D;
			}
			#login_btn_wraper button:focus{outline:none;}
		</style>
		<style>
	a.write:link { 
color:write; 
text-decoration:underline; 
} 
a.write:active { 
color:write; 
text-decoration:none; 
} 
	</style>
	</head>
	
<body>

<div class="w">
    <h1><p>EMMS电子邮件内容监控系统</p></h1> 
  </div>
	<div class="wrap">
	 
		<form method="post" name="form" autoComplete="off" class="J_ajaxForm" action="application/login.php">
		
			<div class="login">
				<ul>
					<li>
						<input class="input" id="username" required name="username" type="text" placeholder="帐号名" title="帐号名" value=""/>
					</li>
					<li>
						<input class="input" id="password" type="password" required name="password" placeholder="密码" title="密码" />
					</li>
				
				</ul>
				<input type="submit" class="btn btn_submit J_ajax_submit_btn" value="登陆" />
					<br></br>
			</div>				
		</form> 
		  <form method="post" name="myform" action="reg.php">		  
		  <input type="submit" class="btn btn_submit J_ajax_submit_btn"  value="注册" />
		  </form> 
		
	</div>
</body>
</html>
