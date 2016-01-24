<!DOCTYPE html>

<html>

<head>
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<!-- Set render engine for 360 browser -->
<meta name="renderer" content="webkit">
<meta charset="utf-8">
<title>EMMS电子邮件内容监测系统</title>

<link href="../test/css/res/theme_blue.css" rel="stylesheet">
 
<link href="../test/css/theme.min.css" rel="stylesheet">
<link href="../test/css/simplebootadmin.css" rel="stylesheet">
<link href="../test/css/font-awesome.min.css" rel="stylesheet" type="text/css">
<link href="../test/css/simplebootadminindex.min.css" rel="stylesheet" >
<style>
.navbar .nav_shortcuts .btn{margin-top: 5px;}

/*-----------------导航hack--------------------*/
.nav-list>li.open{position: relative;}
.nav-list>li.open .back {display: none;}
.nav-list>li.open .normal {display: inline-block !important;}
.nav-list>li.open a {padding-left: 7px;}
.nav-list>li .submenu>li>a {background: #fff;}
.nav-list>li .submenu>li a>[class*="fa-"]:first-child{left:20px;}
.nav-list>li ul.submenu ul.submenu>li a>[class*="fa-"]:first-child{left:30px;}
/*----------------导航hack--------------------*/
</style>
<style>
#think_page_trace_open{left: 0 !important;
right: initial !important;}	

</style>
</head>
<body style="min-width:900px;" screen_capture_injected="true">
	<div class="navbar">
		<div class="navbar-inner">
			<div class="container-fluid">
				<a href="sy.php" class="brand"> <small> 
						EMMS电子邮件内容监测系统
				</small>
				</a>
				<div class="pull-left nav_shortcuts" >
				<a href="syscon.php" >
				    <img class="nav-user-photo" src="../test/imag/2.png" alt="start">
				</a>
				<a href="syscon1.php" >
				    <img class="nav-user-photo" src="../test/imag/3.png" alt="stop">
				</a></div>
				<ul class="nav simplewind-nav pull-right">
					<li class="light-blue">
						<a  href="../test/application/sy.php?logout=out" >
							<img class="nav-user-photo" src="../test/imag/logo-18.png" alt="admin">
							<span class="user-info">
							<small>欢迎,</small><?php session_start();
                                                if(isset($_SESSION['username'])) echo $_SESSION['username'] ; 
                                                else ?> <a href="../test/application/index.php">请登录</a>";
                                               </span>
						</a>
					</li>
				</ul>
			</div>
		</div>
	</div>

	<div class="main-container container-fluid">

		<div class="sidebar" id="sidebar">
			<!-- <div class="sidebar-shortcuts" id="sidebar-shortcuts">
			</div> -->
			<div id="nav_wraper" style="height: 309px; overflow: auto;">
			<ul class="nav nav-list">
				
						<li>
						<a href="#" class="dropdown-toggle">
								 <i class="fa fa-caret-right"></i> 
								<span class="menu-text normal">用户管理</span>	
														</a>
							<ul class="submenu" >
														
									<li>
						<a href="pinfo.php" class="dropdown-toggle">
								<i class="fa fa-caret-right"></i>
								<span>修改用户密码</span>
								<b class="arrow fa fa-angle-right"></b>
							</a>
					</li>
								
							<li>
													
							<a href="confreg.php" class="dropdown-toggle">
								<i class="fa fa-caret-right"></i>
								<span>确定用户注册信息</span>
															</a>
							</li>
							
							<li>
													
							<a href="showuser.php" class="dropdown-toggle">
								<i class="fa fa-caret-right"></i>
								<span>显示用户列表</span>
															</a>
							</li>
							
					</ul>
					<li>
					<a href="logmanage.php" class="dropdown-toggle">
								<i class="fa fa-list normal"></i>
								<span class="menu-text normal">日志管理</span>			
							</a>
						
					
									<li>
						<a href="#" class="dropdown-toggle">
								<i class="fa fa-th normal"></i>
								<span class="menu-text normal">监测设置</span>
														</a>
							
	     <ul class="submenu">
				<li>
					<a href="monitorset.php">
								<i class="fa fa-caret-right"></i>
								<span class="menu-text">添加监测关键字</span>
															</a>
												
					</li>
					
									<li>
				<a href="monitorset1.php">
								<i class="fa fa-caret-right"></i>
								<span class="menu-text">监测关键字列表</span>
															</a>
												
					</li></ul>
					
				<li>
					<a href="#" class="dropdown-toggle">
								<i class="fa fa-th normal"></i>
								<span class="menu-text normal">封堵设置</span>
														</a>
							
							<ul class="submenu">
														<li>
				<a href="plugset.php">
								<i class="fa fa-caret-right"></i>
								<span class="menu-text">添加封堵关键字</span>
															</a>
												
					</li>
					
									<li>
					<a href="plugset1.php">
								<i class="fa fa-caret-right"></i>
								<span class="menu-text">封堵关键字列表</span>
															</a>
												
					</li>				
					</ul>
					<li>
					<a href="#" class="dropdown-toggle">
								<i class="fa fa-th normal"></i>
								<span class="menu-text normal">系统管理</span>
														</a>
							
							<ul class="submenu">
														<li>
				<a href="cmd.php">
								<i class="fa fa-caret-right"></i>
								<span class="menu-text">CPU、内存、硬盘 状态</span>
															</a>
												
					</li>		
					</ul>
			</div>			
		</div>

		<div class="main-content">
			<div class="breadcrumbs" id="breadcrumbs">
				<a id="task-pre" class="task-changebt" style="display: none;">←</a>
				<div id="task-content" style="width: 118px;">
				<ul class="macro-component-tab" id="task-content-inner" style="width: 118px; margin-left: 0px;">
					<li class="macro-component-tabitem noclose" app-id="0"  app-name="首页">
						<span class="macro-tabs-item-text">首页</span>
						</li>
				</ul>	 
				  
				<div style="clear:both;"></div>
				<a id="task-next" class="task-changebt" style="display: none;">→</a>
			</div>
			<div class="page-content" id="content" style="height: 345px;">
			<iframe src="../test/frame/index.php" style="width: 100%; height: 100%; display: block;" frameborder="0" id="appiframe-0" class="appiframe"></iframe>
    </div>
    </div>
</div>
     <!--   <script src="../test/css/res/jquery-1.7.2.min.js"></script>
	
			<script type="text/javascript">
				if("ontouchend" in document) document.write("<script src='__TMPL__res/jquery.mobile.custom.min.js'>"+"<"+"/script>");
			</script>
 
			<script src="../test/css/res/bootstrap.min.js"></script>

			<script src="../test/css/res/prettify.js"></script>

			<script src="../test/css/res/ace-elements.min.js"></script>
			<script src="../test/css/res/ace.min.js"></script>
			<script src="../test/css/res/jquery.nicescroll.js"></script>
			<script src="../test/css/res/script.js"></script>
			<script src="../test/css/res/index.js"></script>-->
	<script src="../test/js/jquery.js"></script>
	<script src="../test/js/bootstrap.min.js"></script>
	<script>
	var ismenumin = $("#sidebar").hasClass("menu-min");
	$(".nav-list").on( "click",function(event) {
		var closest_a = $(event.target).closest("a");
		if (!closest_a || closest_a.length == 0) {
			return
		}
		if (!closest_a.hasClass("dropdown-toggle")) {
			if (ismenumin && "click" == "tap" && closest_a.get(0).parentNode.parentNode == this) {
				var closest_a_menu_text = closest_a.find(".menu-text").get(0);
				if (event.target != closest_a_menu_text && !$.contains(closest_a_menu_text, event.target)) {
					return false
				}
			}
			return
		}
		var closest_a_next = closest_a.next().get(0);
		if (!$(closest_a_next).is(":visible")) {
			var closest_ul = $(closest_a_next.parentNode).closest("ul");
			if (ismenumin && closest_ul.hasClass("nav-list")) {
				return
			}
			closest_ul.find("> .open > .submenu").each(function() {
						if (this != closest_a_next && !$(this.parentNode).hasClass("active")) {
							$(this).slideUp(150).parent().removeClass("open")
						}
			});
		}
		if (ismenumin && $(closest_a_next.parentNode.parentNode).hasClass("nav-list")) {
			return false;
		}
		$(closest_a_next).slideToggle(150).parent().toggleClass("open");
		return false;
	});
	</script>
	<script src="../test/js/sy.js"></script>
</body>
</html>