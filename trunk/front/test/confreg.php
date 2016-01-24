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
	<div id="loading" style="display: none;"><i class="loadingicon"></i><span>正在加载...</span></div>
	<div id="right_tools_wrapper">
		<!--<span id="right_tools_clearcache" title="清除缓存" onclick="javascript:openapp('/www/ThinkCMFX/index.php?g=admin&m=setting&a=clearcache','right_tool_clearcache','清除缓存');"><i class="fa fa-trash-o right_tool_icon"></i></span>-->
		<span id="refresh_wrapper" title="刷新当前页"><i class="fa fa-refresh right_tool_icon"></i></span>
	</div>
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
                                                else echo "shibai";
                                                ?>						</span>
							<i class="fa fa-caret-down"></i>
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
						<div id="nav_wraper" style="height: 347px; overflow: auto;">
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
				<div id="task-content" style="width: 236px;">
				<ul class="macro-component-tab" id="task-content-inner" style="width: 236px; margin-left: 0px;">
					<li class="macro-component-tabitem noclose" app-id="0"  app-name="首页">
						<span class="macro-tabs-item-text">首页</span>
						</li>
				<li class="macro-component-tabitem current" app-id="115Admin" app-name="注册用户"><span class="macro-tabs-item-text">注册用户</span><a class="macro-component-tabclose" href="javascript:void(0)" title="点击关闭标签"><span></span><b class="macro-component-tabclose-icon">×</b></a></li></ul>
				<div style="clear:both;"></div>
				</div>
				<a id="task-next" class="task-changebt" style="display: none;">→</a>
			</div>
	<div class="page-content" id="content" style="height: 345px;">
				<iframe src="../test/frame/index.php" style="width: 100%; height: 100%; display: none;" frameborder="0" id="appiframe-0" class="appiframe"></iframe>
			<iframe style="width:100%;height: 100%;" frameborder="0" class="appiframe" src="../test/frame/reg.php" id="appiframe-115Admin"></iframe>
		</div>

	</div>
	
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






<div id="think_page_trace" style="position: fixed;bottom:0;right:0;font-size:14px;width:100%;z-index: 999999;color: #000;text-align:left;font-family:'微软雅黑';">
<div id="think_page_trace_tab" style="display: none;background:white;margin:0;height: 250px;">
<div id="think_page_trace_tab_tit" style="height:30px;padding: 6px 12px 0;border-bottom:1px solid #ececec;border-top:1px solid #ececec;font-size:16px">
	    <span style="color: rgb(0, 0, 0); padding-right: 12px; height: 30px; line-height: 30px; display: inline-block; margin-right: 3px; cursor: pointer; font-weight: 700;">基本</span>
        <span style="color: rgb(153, 153, 153); padding-right: 12px; height: 30px; line-height: 30px; display: inline-block; margin-right: 3px; cursor: pointer; font-weight: 700;">文件</span>
        <span style="color: rgb(153, 153, 153); padding-right: 12px; height: 30px; line-height: 30px; display: inline-block; margin-right: 3px; cursor: pointer; font-weight: 700;">流程</span>
        <span style="color: rgb(153, 153, 153); padding-right: 12px; height: 30px; line-height: 30px; display: inline-block; margin-right: 3px; cursor: pointer; font-weight: 700;">错误</span>
        <span style="color: rgb(153, 153, 153); padding-right: 12px; height: 30px; line-height: 30px; display: inline-block; margin-right: 3px; cursor: pointer; font-weight: 700;">SQL</span>
        <span style="color: rgb(153, 153, 153); padding-right: 12px; height: 30px; line-height: 30px; display: inline-block; margin-right: 3px; cursor: pointer; font-weight: 700;">调试</span>
    </div>
<div id="think_page_trace_tab_cont" style="overflow:auto;height:212px;padding: 0; line-height: 24px">
		    <div style="display: block;">
    <ol style="padding: 0; margin:0">
	<li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">请求信息 : 2015-11-30 10:28:37 HTTP/1.1 GET : /www/ThinkCMFX/index.php?g=Admin&amp;m=Index&amp;a=index</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">运行时间 : 0.2590s ( Load:0.0100s Init:0.0040s Exec:0.1950s Template:0.0500s )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">吞吐率 : 3.86req/s</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">内存开销 : 2,454.63 kb</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">查询信息 : 48 queries 0 writes </li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">文件加载 : 56</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">缓存信息 : 1 gets 0 writes </li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">配置加载 : 150</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">会话信息 : SESSION_ID=0219bm3dhacip4jrpacq7lpk51</li>    </ol>
    </div>
        <div style="display: none;">
    <ol style="padding: 0; margin:0">
	<li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\index.php ( 1.60 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\api\uc_client\config.inc.php ( 0.07 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\ThinkPHP.php ( 4.71 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Think.class.php ( 12.32 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Storage.class.php ( 1.38 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Storage\Driver\File.class.php ( 3.56 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Mode\common.php ( 2.82 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Common\functions.php ( 52.60 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Common\function.php ( 37.40 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Hook.class.php ( 4.22 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\App.class.php ( 13.46 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Dispatcher.class.php ( 15.08 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Route.class.php ( 13.38 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Controller.class.php ( 10.95 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\View.class.php ( 7.96 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Behavior\BuildLiteBehavior.class.php ( 3.69 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Behavior\ParseTemplateBehavior.class.php ( 3.89 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Behavior\ContentReplaceBehavior.class.php ( 1.91 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Conf\convention.php ( 11.18 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Conf\config.php ( 4.26 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\data\conf\db.php ( 0.32 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Conf\alias.php ( 1.28 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Conf\tags.php ( 0.25 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Lang\zh-cn.php ( 2.57 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Conf\debug.php ( 1.51 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Conf\debug.php ( 0.25 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Behavior\InitHookBehavior.class.php ( 1.39 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Behavior.class.php ( 0.88 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Cache.class.php ( 3.84 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Cache\Driver\File.class.php ( 5.90 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Common\extend.php ( 29.84 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Admin\Conf\config.php ( 0.02 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Admin\Common\function.php ( 0.99 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Behavior\ReadHtmlCacheBehavior.class.php ( 5.75 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Behavior\CheckLangBehavior.class.php ( 2.96 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Lang\zh-cn.php ( 0.45 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Admin\Lang\zh-cn.php ( 0.50 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Admin\Controller\IndexController.class.php ( 0.42 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Controller\AdminbaseController.class.php ( 5.14 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Controller\AppframeController.class.php ( 4.74 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Model.class.php ( 66.78 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Db.class.php ( 5.69 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Db\Driver\Mysql.class.php ( 6.96 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Db\Driver.class.php ( 41.26 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php ( 7.07 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\CommonModel.class.php ( 1.68 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Lib\iAuth.class.php ( 3.45 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Template.class.php ( 28.35 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Template\TagLib\Cx.class.php ( 22.62 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Think\Template\TagLib.class.php ( 9.19 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Lib\Taglib\TagLibSpadmin.class.php ( 2.08 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Lib\Taglib\TagLibHome.class.php ( 2.07 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\data\runtime\Cache\Admin\b99fce69aa3a74f056f3913a79dfb133.php ( 10.05 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Behavior\WriteHtmlCacheBehavior.class.php ( 1.43 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\application\Common\Behavior\TmplStripSpaceBehavior.class.php ( 1.07 KB )</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">D:\xampp\htdocs\www\ThinkCMFX\simplewind\Core\Library\Behavior\ShowPageTraceBehavior.class.php ( 5.27 KB )</li>    </ol>
    </div>
        <div style="display: none;">
    <ol style="padding: 0; margin:0">
	<li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ app_init ] --START--</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">Run Behavior\BuildLiteBehavior [ RunTime:0.000000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">Run Common\Behavior\InitHookBehavior [ RunTime:0.001000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ app_init ] --END-- [ RunTime:0.001000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ app_begin ] --START--</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">Run Behavior\ReadHtmlCacheBehavior [ RunTime:0.000000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">Run Behavior\CheckLangBehavior [ RunTime:0.001000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ app_begin ] --END-- [ RunTime:0.001000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ view_parse ] --START--</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ template_filter ] --START--</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">Run Behavior\ContentReplaceBehavior [ RunTime:0.000000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ template_filter ] --END-- [ RunTime:0.000000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">Run Behavior\ParseTemplateBehavior [ RunTime:0.050000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ view_parse ] --END-- [ RunTime:0.050000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ view_filter ] --START--</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">Run Behavior\WriteHtmlCacheBehavior [ RunTime:0.000000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">Run Common\Behavior\TmplStripSpaceBehavior [ RunTime:0.000000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ view_filter ] --END-- [ RunTime:0.000000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[ app_end ] --START--</li>    </ol>
    </div>
        <div style="display: none;">
    <ol style="padding: 0; margin:0">
	<li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">[8] Undefined variable: ret D:\xampp\htdocs\www\ThinkCMFX\application\Common\Model\MenuModel.class.php 第 164 行.</li>    </ol>
    </div>
        <div style="display: none;">
    <ol style="padding: 0; margin:0">
	<li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SHOW COLUMNS FROM `sp_users` [ RunTime:0.0150s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_users` WHERE ( id=1 ) LIMIT 1   [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SHOW COLUMNS FROM `sp_menu` [ RunTime:0.0200s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 0 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0600s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 109 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 110 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0100s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 111 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 113 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 115 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 126 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 127 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 129 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 131 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 132 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0100s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 133 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 134 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 137 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 139 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 140 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 146 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 147 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 149 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 85 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 86 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 87 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 94 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 100 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 1 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 2 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 4 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 7 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 18 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 25 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 32 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 33 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0100s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 36 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 39 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0100s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 40 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 41 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 42 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 47 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 156 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 53 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0100s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 54 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 62 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 68 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 75 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li><li style="border-bottom:1px solid #EEE;font-size:14px;padding:0 12px">SELECT * FROM `sp_menu` WHERE `parentid` = 83 AND `status` = 1 ORDER BY `listorder` ASC  [ RunTime:0.0000s ]</li>    </ol>
    </div>
        <div style="display: none;">
    <ol style="padding: 0; margin:0">
	    </ol>
    </div>
    </div>
</div>
<script type="text/javascript">
(function(){
var tab_tit  = document.getElementById('think_page_trace_tab_tit').getElementsByTagName('span');
var tab_cont = document.getElementById('think_page_trace_tab_cont').getElementsByTagName('div');
var open     = document.getElementById('think_page_trace_open');
var close    = document.getElementById('think_page_trace_close').childNodes[0];
var trace    = document.getElementById('think_page_trace_tab');
var cookie   = document.cookie.match(/thinkphp_show_page_trace=(\d\|\d)/);
var history  = (cookie && typeof cookie[1] != 'undefined' && cookie[1].split('|')) || [0,0];
open.onclick = function(){
	trace.style.display = 'block';
	this.style.display = 'none';
	close.parentNode.style.display = 'block';
	history[0] = 1;
	document.cookie = 'thinkphp_show_page_trace='+history.join('|')
}
close.onclick = function(){
	trace.style.display = 'none';
this.parentNode.style.display = 'none';
	open.style.display = 'block';
	history[0] = 0;
	document.cookie = 'thinkphp_show_page_trace='+history.join('|')
}
for(var i = 0; i < tab_tit.length; i++){
	tab_tit[i].onclick = (function(i){
		return function(){
			for(var j = 0; j < tab_cont.length; j++){
				tab_cont[j].style.display = 'none';
				tab_tit[j].style.color = '#999';
			}
			tab_cont[i].style.display = 'block';
			tab_tit[i].style.color = '#000';
			history[1] = i;
			document.cookie = 'thinkphp_show_page_trace='+history.join('|')
		}
	})(i)
}
parseInt(history[0]) && open.click();
(tab_tit[history[1]] || tab_tit[0]).click();
})();
</script>
</body>
</html>