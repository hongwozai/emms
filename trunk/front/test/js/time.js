//function fTime(iHour,iSec,iMin)
//{
// return iHour+iSec+iMin;
//}
//��ǰ����
//<input type="text" size="10" name="datBegin" value="<%=formatdatetime(dateandtime_start,2)%>" maxlength="10"   readonly  onclick="fPopCalendar(datBegin,datBegin);return false"> ��<%h_time(int(hour(dateandtime_start))),23,"datHourBegin"%> ʱ<%h_time(int(minute(dateandtime_start))),59,"datMinuteBegin"%>��(�� �ڸ�ʽ:yyyy-mm-dd)
function getNowDate()
{
   var nn=new Date();
   year1=nn.getYear();
   mon1=nn.getMonth()+1;
   date1=nn.getDate();
   var monstr1;
   var datestr1
   if(mon1<10) 
    monstr1="0"+mon1;
   else
    monstr1=""+mon1;
     
   if(date1<10) 
     datestr1="0"+date1;
   else
     datestr1=""+date1;
   return year1+"-"+monstr1+"-"+datestr1;
}
//Ŀ������
function getlastweekDate()
{
   var nn=new Date();
   year1=nn.getYear();
   mon1=nn.getMonth()+1;
   date1=nn.getDate();
   
   var mm=new Date(year1,mon1-1,date1);
   var tmp1=new Date(2000,1,1);
   var tmp2=new Date(2000,1,15);
   var ne=tmp2-tmp1;
   var mm2=new Date();
   mm2.setTime(mm.getTime()-ne);

 
   
   
   year2=mm2.getYear();
   mon2=mm2.getMonth()+1;
   date2=mm2.getDate();
    
    
     if(mon2<10) 
    monstr2="0"+mon2;
   else
    monstr2=""+mon2;
     
   if(date2<10) 
     datestr2="0"+date2;
   else
     datestr2=""+date2;
 
   
    return year2+"-"+monstr2+"-"+datestr2;
}

 

var gdCtrl = new Object();
var goSelectTag = new Array();
var gcGray   = "#808080";
var gcToggle = "#FB8664";
var gcBG = "#e5e6ec";
var previousObject = null;

var gdCurDate = new Date();
var giYear = gdCurDate.getFullYear();
var giMonth = gdCurDate.getMonth()+1;
var giDay = gdCurDate.getDate();
function fSetToday()  //ֱ�ӻ�ȡ���ڵ�ʱ��
{
 
}

function fSetDate(iYear, iMonth, iDay){                         

  VicPopCal.style.visibility = "hidden";
  if ((iYear == 0) && (iMonth == 0) && (iDay == 0)){
   gdCtrl.value = "";
  }else{
   iMonth = iMonth + 100 + "";
   iMonth = iMonth.substring(1);
   iDay   = iDay + 100 + "";
   iDay   = iDay.substring(1);

   if(gdCtrl.tagName == "INPUT"){
      gdCtrl.value = iYear+"-"+iMonth+"-"+iDay+"";
   }else{
      gdCtrl.innerText = iYear+"-"+iMonth+"-"+iDay+ "";
   }
  }
  
  for (i in goSelectTag)
   goSelectTag[i].style.visibility = "visible";
  goSelectTag.length = 0;
  
  window.returnValue=gdCtrl.value;
  //window.close();

}

function HiddenDiv()
{
 var i;
  VicPopCal.style.visibility = "hidden";
  for (i in goSelectTag)
   goSelectTag[i].style.visibility = "visible";
  goSelectTag.length = 0;

}
function fSetSelected(aCell){
  var iOffset = 0;
  var iYear = parseInt(tbSelYear.value);
  var iMonth = parseInt(tbSelMonth.value);
  
  aCell.bgColor = gcBG;
  with (aCell.children["cellText"]){
   var iDay = parseInt(innerText);
   if (color==gcGray)
  iOffset = (Victor<10)?-1:1;
 iMonth += iOffset;
 if (iMonth<1) {
  iYear--;
  iMonth = 12;
 }else if (iMonth>12){
  iYear++;
  iMonth = 1;
 }
  }
  fSetDate(iYear, iMonth, iDay);
}

function Point(iX, iY){
 this.x = iX;
 this.y = iY;
}

function fBuildCal(iYear, iMonth) {
  var aMonth=new Array();
  for(i=1;i<7;i++)
   aMonth[i]=new Array(i);
  
  var dCalDate=new Date(iYear, iMonth-1, 1);
  var iDayOfFirst=dCalDate.getDay();
  var iDaysInMonth=new Date(iYear, iMonth, 0).getDate();
  var iOffsetLast=new Date(iYear, iMonth-1, 0).getDate()-iDayOfFirst+1;
  var iDate = 1;
  var iNext = 1;

  for (d = 0; d < 7; d++)
 aMonth[1][d] = (d<iDayOfFirst)?-(iOffsetLast+d):iDate++;
  for (w = 2; w < 7; w++)
   for (d = 0; d < 7; d++)
  aMonth[w][d] = (iDate<=iDaysInMonth)?iDate++:-(iNext++);
  return aMonth;
}

function fDrawCal(iYear, iMonth, iCellHeight, sDateTextSize) {
  var WeekDay = new Array("周日","周一","周二","周三","周四","周五","周六");
  var styleTD = " bgcolor='"+gcBG+"' bordercolor='"+gcBG+"' valign='middle' align='center' height='"+iCellHeight+"' style='font:bold arial "+sDateTextSize+";";            //Coded by Hcy email:hcy110@263.net

  with (document) {
 write("<tr>");
 for(i=0; i<7; i++){
  write("<td "+styleTD+"color:maroon' >"+ WeekDay[i] + "</td>");
 }
 write("</tr>");

   for (w = 1; w < 7; w++) {
  write("<tr>");
  for (d = 0; d < 7; d++) {
   write("<td id=calCell "+styleTD+"cursor:hand;' onMouseOver='this.bgColor=gcToggle' onMouseOut='this.bgColor=gcBG' onclick='fSetSelected(this)'>");
   write("<font id=cellText Victor='Hcy_Flag'> </font>");   
   write("</td>")
  }
  write("</tr>");
 }
  }
}

function fUpdateCal(iYear, iMonth) {
  myMonth = fBuildCal(iYear, iMonth);
  var i = 0;
  for (w = 0; w < 6; w++)
 for (d = 0; d < 7; d++)
  with (cellText[(7*w)+d]) {
   Victor = i++;
   if (myMonth[w+1][d]<0) {
    color = gcGray;
    innerText = -myMonth[w+1][d];
    
   }else{
    color = ((d==0)||(d==6))?"red":"black";
    innerText = myMonth[w+1][d];
       
   }
  }
}

function fSetYearMon(iYear, iMon){
  tbSelMonth.options[iMon-1].selected = true;
  for (i = 0; i < tbSelYear.length; i++)
 if (tbSelYear.options[i].value == iYear)
  tbSelYear.options[i].selected = true;
  fUpdateCal(iYear, iMon);
}

function fPrevMonth(){
  var iMon = tbSelMonth.value;
  var iYear = tbSelYear.value;
  
  if (--iMon<1) {
   iMon = 12;
   iYear--;
  }
  
  fSetYearMon(iYear, iMon);
}

function fNextMonth(){
  var iMon = tbSelMonth.value;
  var iYear = tbSelYear.value;
  
  if (++iMon>12) {
   iMon = 1;
   iYear++;
  }
  
  fSetYearMon(iYear, iMon);
}

function fToggleTags(){
  with (document.all.tags("SELECT")){
  for (i=0; i<length; i++)
   if ((item(i).Victor!="Won")&&fTagInBound(item(i))){
    item(i).style.visibility = "hidden";
    goSelectTag[goSelectTag.length] = item(i);
   }
  }
}

function fTagInBound(aTag){
  with (VicPopCal.style){
   var l = parseInt(left);
   var t = parseInt(top);
   var r = l+parseInt(width);
   var b = t+parseInt(height);
 var ptLT = fGetXY(aTag);
 return !((ptLT.x>r)||(ptLT.x+aTag.offsetWidth<l)||(ptLT.y>b)||(ptLT.y+aTag.offsetHeight<t));
  }
}

function fGetXY(aTag){
  var oTmp = aTag;
  var pt = new Point(0,0);
  do {
   pt.x += oTmp.offsetLeft;
   pt.y += oTmp.offsetTop;
   oTmp = oTmp.offsetParent;
  } while(oTmp.tagName!="BODY");
  return pt;
}

// Main: popCtrl is the widget beyond which you want this calendar to appear;
//       dateCtrl is the widget into which you want to put the selected date.
// i.e.: <input type="text" name="dc" style="text-align:center" readonly><INPUT type="button" value="V" onclick="fPopCalendar(dc,dc);return false">
function fPopCalendar(popCtrl, dateCtrl,strDate){
  if (popCtrl == previousObject){
    if (VicPopCal.style.visibility == "visible"){
    HiddenDiv();
    return true;
   }
   
  }
  previousObject = popCtrl;
  gdCtrl = dateCtrl;
  fInitialDate(strDate);
  fSetYearMon(giYear, giMonth); 
  var point = fGetXY(popCtrl);
  with (VicPopCal.style) {
   left = point.x+10;   //�������ڵ�����
 top  = point.y+popCtrl.offsetHeight+15;
 width = VicPopCal.offsetWidth;
 width = 213; //
 height = VicPopCal.offsetHeight;
 height = 235;
 fToggleTags(point);  
 visibility = 'visible';
  }
}

// Added by Han Chen
function fInitialDate(strDate){
 if( strDate == null || strDate.length != 10 )
  return false;

 var sYear  = strDate.substring(0,4);
 var sMonth = strDate.substring(5,7);
 var sDay   = strDate.substring(8,10);

 if( sMonth.charAt(0) == '0' ) { sMonth = sMonth.substring(1,2); }
 if( sDay.charAt(0)   == '0' ) { sDay   = sDay.substring(1,2);   }

 var nYear  = parseInt(sYear );
 var nMonth = parseInt(sMonth);
 var nDay   = parseInt(sDay  );
 
 if ( isNaN(nYear ) ) return false;
 if ( isNaN(nMonth) ) return false;
 if ( isNaN(nDay  ) ) return false;

 var arrMon = new Array(12);
 arrMon[ 0] = 31; arrMon[ 1] = nYear % 4 == 0 ? 29:28;
 arrMon[ 2] = 31; arrMon[ 3] = 30;
 arrMon[ 4] = 31; arrMon[ 5] = 30;
 arrMon[ 6] = 31; arrMon[ 7] = 31;
 arrMon[ 8] = 30; arrMon[ 9] = 31;
 arrMon[10] = 30; arrMon[11] = 31;

 if ( nYear  < 1900 || nYear > 2100 )   return false;
 if ( nMonth < 1 || nMonth > 12 )    return false;
 if ( nDay < 1 || nDay > arrMon[nMonth - 1] ) return false;

 giYear  = nYear;
 giMonth = nMonth;
 giDay   = nDay;
 return true;
}

var gMonths = new Array("1月","2月","3月","4月","5月","6月","7月","8月","9月","10月","11月","12月");

with (document) {
write("<Div id='VicPopCal' style='POSITION:absolute;VISIBILITY:hidden;border:1px ridge;z-index:200;'>");
write("<table border='0' bgcolor='#cccccc'>");
write("<TR>");
write("<td valign='middle' align='center'><input type='button' name='PrevMonth' value='<' style='height:20;width:20;FONT:bold' onClick='fPrevMonth()'>");
write("&nbsp;<SELECT name='tbSelYear' onChange='fUpdateCal(tbSelYear.value, tbSelMonth.value)' Victor='Won'>");
for(i=1950;i<2030;i++)
 write("<OPTION value='"+i+"'>"+i+" 年</OPTION>");
write("</SELECT>");
write("&nbsp;<select name='tbSelMonth' onChange='fUpdateCal(tbSelYear.value, tbSelMonth.value)' Victor='Won'>");
for (i=0; i<12; i++)
 write("<option value='"+(i+1)+"'>"+gMonths[i]+"</option>");
write("</SELECT>");

 


write("&nbsp;<input type='button' name='PrevMonth' value='>' style='height:20;width:20' onclick='fNextMonth()'>");
write("</td>");
write("</TR><TR>");
write("<td align='center'>");
write("<DIV style='background-color:teal'><table width='100%' border='0'>");
fDrawCal(giYear, giMonth, 0, '0');
write("</table></DIV>");
write("</td>");
write("</TR><TR><TD align='center'>");



//write("&nbsp;<select name='tbSelHour' onChange='fTime(tbSelHour,tbSelSec,tbSelMin)' Victor='Won'>");
//for(i=0;i<24;i++)
// {
// if(i<10)
// {
//  write("<OPTION value='0"+i+"'>"+i+"ʱ</OPTION>");
// }
// else
// {
//  write("<OPTION value='"+i+"'>"+i+"ʱ</OPTION>");
// }
// }
//write("</SELECT>");

//write("&nbsp;<select name='tbSelSec' onChange='fTime(tbSelHour,tbSelSec,tbSelMin)' Victor='Won'>");
//for(i=0;i<60;i++)
// if(i<10)
// {
//  write("<OPTION value='0"+i+"'>"+i+"��</OPTION>");
// }
// else
// {
//  write("<OPTION value='"+i+"'>"+i+"��</OPTION>");
// }
//write("</SELECT>");

//write("&nbsp;<select name='tbSelMin' onChange='fTime(tbSelHour,tbSelSec,tbSelMin)' Victor='Won'>");
//for(i=0;i<60;i++)
// if(i<10)
// {
//  write("<OPTION value='0"+i+"'>"+i+"��</OPTION>");
// }
// else
// {
//  write("<OPTION value='"+i+"'>"+i+"��</OPTION>");
// }
//write("</SELECT>");



write("<TABLE width='100%'><TR><TD align='center'>");
//write("<font size='2'  onclick='fSetDate(0,0,0)' onMouseOver='this.style.color=gcToggle' onMouseOut='this.style.color=0'>&nbsp;���</font>");  
//B style='cursor:hand'���滻Ϊfont size='2'
write("</td><td algin='center'>");
write("<font size='2'  onclick='fSetDate(giYear,giMonth,giDay)' onMouseOver='this.style.color=gcToggle' onMouseOut='this.style.color=0'>今天日期: "+giYear+"-"+giMonth+"-"+giDay+"</font>");
//B style='cursor:hand'���滻Ϊfont size='2'
write("</td></tr></table>");
write("</TD></TR>");
write("</TABLE></Div>");
}