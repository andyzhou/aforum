//create and init http_request
function xmlHttpRequest()
{
	var req = null;

	if(typeof XMLHttpRequest != "undefined")
	{
		req = new XMLHttpRequest();

		if(req.overrideMimeType)
		{
			req.overrideMimeType("text/xml");
		}
	}

	if(!req && typeof ActiveXObject != "undefined")
	{
		try
		{
			req=new ActiveXObject("Msxml2.XMLHTTP");
		}
		catch(e)
		{
			try
			{
				req=new ActiveXObject("Microsoft.XMLHTTP");
			}
			catch(e2)
			{
				try{
					req=new ActiveXObject("Msxml2.XMLHTTP.4.0");
				}
				catch(e3)
				{
					req=null;
				}
			}
		}
	}

	if(!req && window.createRequest)
	{
		req = window.createRequest();
	}

	return req;
}


//split string to array
function splitToArray(str, split_char)
{
	var tmp_array = new Array();
	
	if(str == '' || split_char == '')
	{
		return tmp_array;
	}

	if(str.indexOf(split_char != -1))
	{
		tmp_array = str.split(split_char);
	}
	
	return tmp_array;
}


//preload loading image
function loadBasicImg()
{
	//var img1 = new Image
}


//show loading or sending page
function showLoadSending(obj, loadImgType, load_send, extString)
{
	var load_img = "";
	var load_msg = "";

	switch(loadImgType)
	{
		case 1:
			load_img = "l_1.gif";
			break;
		case 2:
			load_img = "l_2.gif";
			break;
		case 3:
			load_img = "l_3.gif";
			break;
		case 4:
			load_img = "l_4.gif";
			break;
		case 5:
			load_img = "l_5.gif";
			break;
		case 6:
			load_img = "l_6.gif";
			break;
		case 7:
			load_img = "l_7.gif";
			break;
		case 8:
			load_img = "l_8.gif";
			break;
		default:
			load_img = "l_1.gif";
			break;
	}

	if(load_send == 2)
	{
		load_msg = "saving...";
	}
	else
	{
		load_msg = "loading...";
	}

	var loading_img_str = "<img src='/image/loading/"+load_img+"'> "+load_msg + "\r\n<br>"+extString;

	//document.getElementById(obj).parentNode.style.display = "";
	//document.getElementById(obj).innerHTML = loading_img_str;
	//document.getElementById(obj).style.visibility = "visible";

	//obj.parentNode.style.display = "";
	obj.innerHTML = loading_img_str;
	obj.style.visibility = "visible";
}


//check string length, support chinese
function strLenCheck(str, hasChinese)
{
	if(typeof(str) == 'undefined' || str == '')
	{
		return 0;
	}

	var num = 0;
	var len = str.length;

	for(var i = 0; i < len; i++)
	{
		num++;
		if(str.charCodeAt(i)>255)
		{
			hasChinese = true;
			num++;
		}
	}

	return num;
}


//get check value to array
function chkToArray(obj)
{
	var tmp_arr = new Array();

	var obj = document.getElementsByName(obj);
	var objlen = obj.length;

	var i = 0;
	var k = 0;

	for(i=0, k = 0; i< objlen; i++)
	{
		if(obj[i].checked==true)
		{
			//alert("你选择了第"+(i+1)+"个checkbox哦！");
			tmp_arr[k] = obj[i].value;
			k++;
		}	
	}

	return tmp_arr;
}


//generate gen select list
//return select string
function genCommSelectList(dataStr, curOptionId)
{
	//split server data
	var sigRecStr = "";
	var tmpStr = "";
	var selectStr = "";
	var recArr = new Array();
	var selecedOptionStr = "";
	
	if(typeof(dataStr) == 'undefined' || dataStr == '')
	{
	  return selectStr;
	}

	recArr = splitToArray(dataStr, '|');

	for(var i = 0; i < recArr.length; i++)
	{
		sigRecStr = recArr[i];

		if(typeof(sigRecStr) == 'undefined' || sigRecStr == '')
		{
		  continue;
		}

		var colArr = splitToArray(sigRecStr, ':');

		if(colArr[0] == curOptionId)
		{
			selecedOptionStr = "selected";
		}
		else
		{
			selecedOptionStr = "";
		}

		tmpStr = "<option value='"+colArr[0]+"' "+selecedOptionStr+">"+colArr[1]+"</option>\n";

		selectStr += tmpStr;
	}

	return selectStr;
}



//post data to server
//httpReq		HTTP请求对象
//reqType		请求类型 GET?POST
//reqUrl		请求URL
//reqParam		请求参数
//cmdFuncName	回调函数
//pageDiv		页面div id
//extParamArr	扩展参数数组
function postDataToServer(httpReq, reqType, reqUrl,
						  reqParam, cmdFuncName, pageDiv,
						  extParamArr)
{
	httpReq.open(reqType ? reqType : "POST", reqUrl, true);

	var _curPageDiv		= pageDiv ? pageDiv : null;
	var _extParamArr	= extParamArr ? extParamArr : null;

	if(reqType == "POST")
	{
		try
		{
			httpReq.setRequestHeader("Method", "POST " + reqUrl + " HTTP/1.1");
			httpReq.setRequestHeader("Content-Type","application/x-www-form-urlencoded;charset=gb2312");
		}
		catch(e)
		{
			alert("Your browser does not support asynchronous requests using POST.");
			return false;
		}
	}
	
	httpReq.onreadystatechange = function()
	{
		if(httpReq.readyState != 4)
		{
			return;
		}

		//if(httpReq.status == 200)
		if(httpReq.readyState == 4)
		{
			//调用回调函数
			//alert('state:'+httpReq.readyState);
			//alert('cmdFuncName:'+cmdFuncName);
			//alert(httpReq.responseText);
			eval(cmdFuncName(httpReq.responseText, _curPageDiv, _extParamArr));
		}
	};

	//当请求方式为POST时，可以传参数
	//httpReq.send(isPostType ? encodeURIComponent(reqParam) : null);
	httpReq.send(reqParam ? reqParam : null);
}




//split data and generate gen double array
//return array object
function splitDataToArr(dataStr)
{
	//split server data
	var sigRecStr = "";
	var tmpStr = "";
	var selectStr = "";
	var recArr = new Array();
	var returnArr = new Array();
	var i = 0, k = 0;
	var colArrLen = 0;
	
	if(typeof(dataStr) == 'undefined' || dataStr == '')
	{
		return returnArr;
	}

	recArr = splitToArray(dataStr, '|');
	returnArr = new Array(recArr.length);

	for(i = 0, k = 0; i < recArr.length; i++)
	{
		sigRecStr = "";
		sigRecStr = recArr[i];

		if(typeof(sigRecStr) == 'undefined' || sigRecStr == '')
		{
		  continue;
		}

		var colArr = new Array();

		colArr = splitToArray(sigRecStr, ':');
		colArrLen = colArr.length;

		if(typeof(colArr) == 'undefined' || colArrLen == 0)
		{
			continue;
		}

		returnArr[k] = new Array(colArrLen);

		for(var m = 0; m < colArrLen; m++)
		{
			returnArr[k][m] = colArr[m];
			returnArr[k][m] = colArr[m];
		}

		k++;
	}

	return returnArr;
}


//load city channal
function loadCity(chann, city)
{
	if(typeof(city) == 'undefined' || city == '')
	{
		return;
	}

	window.location = "/"+chann+"/"+city;
}


//show ajax msg div window
//paramArray param array
//paramArray[0]		parant div name
//paramArray[1]		sub div id name
//paramArray[2]		sub div innerHTML
function ajaxMsgWindow(paramArray)
{
	var navigatorName = "Microsoft Internet Explorer";
	var paramArrayLen = paramArray.length;
	
	if(paramArrayLen < 3)
	{
		return;
	}

	var parentDivObj = document.getElementById(paramArray[0]);

	if(typeof(parentDivObj) == 'undefined')
	{
		return;
	}

	var subDivObj = document.getElementById(paramArray[1]);
	var tmpDivStr = '<img src="/image/loading/l_1.gif"> '+paramArray[2];

	//alert(subDivObj);

	//if(typeof(subDivObj) == 'undefined' || typeof(subDivObj) == null || )
	if(typeof(subDivObj) == 'undefined' || !subDivObj)
	{
		//var subDivStr = '<div id="'+msgDiv+'">'+tmpDivStr+'</div>\n';
		
		var subDiv = document.createElement("div");

		//alert(navigator.appName);

		subDiv.id = paramArray[1];
		subDiv.name = paramArray[1];

		if(navigator.appName == navigatorName)
		{
			//ie browser
			subDiv.style.position="relative";
			subDiv.style.width  = paramArray[3] ? paramArray[3] : "130px";
			subDiv.style.height = paramArray[4] ? paramArray[4] : "20px";
			
			//subDiv.style.top  =	parentDivObj.offsetHeight - 25;
			//subDiv.style.left =	parentDivObj.offsetLeft;
			
			subDiv.style.top  =	0;
			subDiv.style.left =	0;
		}
		else
		{
			//other browser
			subDiv.style.position="absolute";
			subDiv.style.width  = paramArray[3] ? paramArray[3]+"!important" : "130px!important";
			subDiv.style.height = paramArray[4] ? paramArray[4]+"!important" : "20px!important";

			
			subDiv.style.top  =	parentDivObj.offsetHeight;
			subDiv.style.left =	parentDivObj.offsetLeft;
		}

		subDiv.style.display = "none";
		subDiv.style.padding = "5px 5px 2px";
		subDiv.style.background = "#FAFAFA";
		subDiv.style.border="blue 1px solid";
		subDiv.style.color = "#7C7C7C";

		//sub div postion, x/y value
		//subDiv.style.top  =	parentDivObj.offsetHeight + 5;
		//subDiv.style.left =	parentDivObj.offsetLeft + 5;
		//subDiv.style.top  =	parentDivObj.offsetHeight;
		//subDiv.style.left =	parentDivObj.offsetLeft;

		subDiv.innerHTML = tmpDivStr;

		subDiv.style.display = "block";
		parentDivObj.appendChild(subDiv, null);
	}
	else
	{
		subDivObj.style.display="block";
		subDivObj.innerHTML = tmpDivStr;
	}
}


/*
function show_pop_div(win_title, input_title)
{
	if(typeof(win_title) == 'undefined' || win_title == '')
	{
		return;
	}

	if(typeof(input_title) == 'undefined' || input_title == '')
	{
		return;
	}

	document.write('<div id="layer_no" class="pop_div" style="display:none">\n');
	document.write('<iframe  class="frame_style" src=""  frameborder="0" scrolling="no"></iframe>\n');
	document.write('<h1>'+win_title+'</h1>\n');
	document.write('<div id="content"></div>\n');
	document.write('<div class="close"><input name="" type="button"  value="'+input_title+'" onclick="close_div()"></div>\n');
	document.write('</div>\n');
}
*/

function pop_div(text){
	var str_text=text;
	layer_no.style.display="";
	document.getElementById("content").innerHTML=str_text;
}

function close_div(){
	layer_no.style.display="none";
}



