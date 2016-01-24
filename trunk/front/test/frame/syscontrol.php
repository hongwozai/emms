<!DOCTYPE html>
<html>
<head>
<admintpl file="header"/>
<style>
.home_info li em {
float: left;
width: 100px;
font-style: normal;
}
li {
list-style: none;
}

h4.well {
min-height: 20px;
padding: 19px;
margin-bottom: 20px;
background-color: #ecf0f1;
border: 1px solid #d7e0e2;
-webkit-border-radius: 3px;
-moz-border-radius: 3px;
border-radius: 3px;
-webkit-box-shadow: inset 0 1px 1px rgba(0,0,0,0.05);
-moz-box-shadow: inset 0 1px 1px rgba(0,0,0,0.05);
box-shadow: inset 0 1px 1px rgba(0,0,0,0.05);
}
</style>
<script type="text/javascript">

function ctrSys() {
    var node = document.getElementById("info");
	var xhr;
	if(window.ActiveXObject){
			xhr = new ActiveXObject("Microsoft.XMLHTTP");
		}else if(window.XMLHttpRequest)
		{
			xhr = new XMLHttpRequest();
		}
    	xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
        	//alert(xhr.readyState);
            if (xhr.status == 200) {
                node.innerHTML = xhr.responseText;
            } else {
                echo("socket error!\n");
            }
        }
    }
        
    xhr.open('GET', '../socket.php?info='+'S\n',true);
    xhr.send();
} 
//setInterval(ctrSys);
</script>
</head>

<body onload="ctrSys()">

<div class="wrap">  
      <h4 class="well">系统状态</h4>
      <div class="home_info" id="info">
      
      </div>
   </div>
</body>
</html>
