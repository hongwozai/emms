<!DOCTYPE html>
<html>
<admintpl file="header"/>

<head>
<style>
.home_info li em {
float: left;
width: 100px;
font-style: normal;
}
li {
list-style: none;
}
.wrap {
padding: 20px 20px 70px;
}
.form-horizontal .control-group {
margin-bottom: 20px;
}
.control-group {
margin-bottom: 10px;
}
.form-horizontal .control-label {
float: left;
width: 160px;
padding-top: 5px;
text-align: right;
}
label {
display: block;
margin-bottom: 5px;
}
.form-horizontal .controls {
margin-left: 180px;
}
user agent stylesheetdiv {
display: block;
}
.form-horizontal .form-actions {
padding-left: 180px;
}
.form-actions {
background-color: #dde4e6;
border-top: 0;
-webkit-border-radius: 3px;
-moz-border-radius: 3px;
border-radius: 3px;
}

.nav-tabs {
border-bottom: 1px solid #ddd;
}
.nav-tabs, .nav-pills {
}
.nav {
margin-bottom: 20px;
margin-left: 0;
list-style: none;
}

fieldset {
padding: 0;
margin: 0;
border: 0;
}
user agent stylesheetfieldset {
display: block;
-webkit-margin-start: 2px;
-webkit-margin-end: 2px;
-webkit-padding-before: 0.35em;
-webkit-padding-start: 0.75em;
-webkit-padding-end: 0.75em;
-webkit-padding-after: 0.625em;
border: 2px groove threedface;
border-image-source: initial;
border-image-slice: initial;
border-image-width: initial;
border-image-outset: initial;
border-image-repeat: initial;
min-width: -webkit-min-content;
}
.white { 
color: #606060; 
border: solid 1px #b7b7b7; 
background: #fff; 
background: -webkit-gradient(linear, left top, left bottom, from(#fff), to(#ededed)); 
background: -moz-linear-gradient(top, #fff, #ededed); 
filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#ffffff', endColorstr='#ededed'); 
	width:100px;height:30px;
} 
</style>
</head>
<body>
<div class="wrap">
  <ul class="nav nav-tabs">
        <li><h4>修改用户密码</h4></li>
      </ul>
  <div class="common-form">
    <form class="form-horizontal J_ajaxForm" method="post" action="../application/modify.php">
        <fieldset class="btn_wrap">
          <div class="control-group">
            <label class="control-label" for="input01">原始密码:</label>
            <div class="controls">
              <input type="password" class="input-xlarge" id="input01" name="old_password">
            </div>
          </div>
          <div class="control-group">
            <label class="control-label" for="input01">新密码:</label>
            <div class="controls">
              <input type="password" class="input-xlarge" id="input01" name="password">
            </div>
          </div>
          <div class="control-group">
            <label class="control-label" for="input01">重复新密码:</label>
            <div class="controls">
              <input type="password" class="input-xlarge" id="input01" name="repassword">
            </div>
          </div>
          <div class="form-actions">
            <button type="submit" name="submit" class="white">更新</button>
          </div>
        </fieldset>
      </form>
  </div>
</div>
</body>
</html>
