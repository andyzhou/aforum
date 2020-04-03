

//\n convert into <br>
function nl2br(src_str)
{
	var target_intro = "";
	var reg = /\r|\n/g;
	target_intro = src_str.replace(reg, '<br>');
	return target_intro;
}

//str replace
function br2nl(src_str)
{
	var target_str = "";
	target_str = src_str.replace(/<br>/g, "\r\n");
	return target_str;
}

function msgBox(msg, obj)
{
	alert(msg);
	obj.focus();
	return;
}


function tagCheck(tagObj)
{
	var tmp_str = tagObj.value;

	if(typeof(tmp_str) == 'undefined' && tmp_str == '')
	{
		return true;
	}

	var has_chinese = false;
	var str_len = strLenCheck(tmp_str, has_chinese);

	if(str_len > 60)
	{
		msgBox('tag lenth max 60', tagObj);
		return false;
	}

	//check blank character
	var len = tmp_str.indexOf(" ");

	if(len <= 0)
	{
		return true;
	}

	//split key word
	var key_arr = tmp_str.split(" ");

	var arr_len = key_arr.length;

	if(arr_len > 5)
	{
		msgBox('tag max 5', tagObj);
		return false;
	}

	for(var i = 0; i < arr_len; i++)
	{
		var tmp_arr_val = key_arr[i];

		//check signal key
		var tmp_arr_val_len = strLenCheck(tmp_arr_val, has_chinese);

		if(tmp_arr_val_len > 10)
		{
			msgBox('the '+(i+1)+' tag length exceed 10 characters', tagObj);
			return false;
		}
	}

	return true;
}

//check string length
function checkStrLen(str, len)
{
	var num = 0;

	if(str == '')
	{
		return false;
	}

	if(str.length <= 0)
	{
		return false;
	}

	for(var i=0; i<str.length;i++)
	{
		num++;
		if(str.charCodeAt(i)>255)
		{
			num++;
		}
	}

	if(num > len)
	{
		return true;
	}

	return false;
}

//trim
function trim(inputString)
{    
  if (typeof inputString != "string") { return inputString; }
  var retValue = inputString;
  var ch = retValue.substring(0, 1);
  while (ch == " ") { 
	  retValue = retValue.substring(1, retValue.length);
	  ch = retValue.substring(0, 1);
  }
  ch = retValue.substring(retValue.length-1, retValue.length);
  while (ch == " ") {
	 retValue = retValue.substring(0, retValue.length-1);
	 ch = retValue.substring(retValue.length-1, retValue.length);
  }
  while (retValue.indexOf("  ") != -1) { 
	 retValue = retValue.substring(0, retValue.indexOf("  ")) + retValue.substring(retValue.indexOf("  ")+1, retValue.length); 
  }
  return retValue;
}


function getFileType(fileName)
{
	var fileExtName = "";

	if(fileName.lastIndexOf(".") == -1)
	{
		return fileExtName;
	}

	fileExtName = fileName.substring(fileName.lastIndexOf(".")+1,fileName.length).toLowerCase();

	return fileExtName;
}

function mailbox_check(emailaddr)
{

    var at_character;
	var mail_check=true;
    
	at_character=emailaddr.indexOf('@');

	if(emailaddr.length < 6)
	{
	  mail_check=false;
	}

	if(at_character <= 0)
	{
	   mail_check=false;
	}

  	if(emailaddr.search(new RegExp("^(([0-9a-zA-Z]+)|([0-9a-zA-Z]+[_.0-9a-zA-Z-]*[0-9a-zA-Z]+))@([a-zA-Z0-9-]+[.])+([a-zA-Z]{2,4})$","gi")) == -1)
	{
	   mail_check = false;
	}

    if(mail_check==false)
	{
	   return false;
	}

	return true;
}


//chinese check, if suit, return true.
function CnWordRegCheck(obj)
{
 var str = obj.value;
 var reg=/^[\u4e00-\u9fa5](\s*[\u4e00-\u9fa5])*$/;
 var flag = reg.test(str);
 //alert("1:"+flag);
 return flag;
}


function SpecialWordRegCheck(obj)
 {
  var reg= /[^a-zA-Z0-9_]/g;
  var str = obj.value;
  var flag= reg.test(str);
  //alert("2:"+flag);
  return flag;
 }

function msgBox(msg)
{
	//pop_div(msg);
	alert(msg);
	return;
}

function ajaxMsg(msg, fobj)
{
	if(typeof(msg) == "undefined" || msg == '')
	{
		return;
	}

	var msgDiv = document.getElementById("servMsg");

	msgDiv.style.display = "";
	msgDiv.innerHTML = msg;

	if(typeof(fobj) != "undefined")
	{
		fobj.focus();
	}
}
