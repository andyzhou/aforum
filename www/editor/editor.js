sysInit();
	var href=location.href;
	var baseurl=href.slice(0,href.lastIndexOf("/"))+"/imgs/";
	function sysInit(){
		var appStr=navigator.userAgent.toLowerCase();
		window.$=function(obj){return typeof(obj)=="string"?document.getElementById(obj):obj}
		window.browser={
			msie:/msie/.test(appStr)&&!/opera/.test(appStr),
			mozilla:/mozilla/.test(appStr)&&!/(compatible|webkit)/.test(appStr),
			safari:/webkit/.test(appStr),
			opera:/opera/.test(appStr),
			firefox:/firefox/.test(appStr)
		}
		window.ow=function(win){return $(win).contentWindow}
		window.oHide=function(obj,type){if($(obj)){(typeof(type)=="number"&&type==1)?$(obj).style.visibility="hidden":$(obj).style.display="none"}}
		window.oShow=function(obj,type){if($(obj)){(typeof(type)=="number"&&type==1)?$(obj).style.visibility="visible":$(obj).style.display=type?type:"block"}}
		window.docWidth=function(){return Math.max(document.body.clientWidth,document.documentElement.clientWidth)}
		window.docHeight=function(){return Math.max(document.body.clientHeight,document.documentElement.clientHeight)}
		String.prototype.trim=function(){return this.replace(/(^[\s]*)|([\s]*$)/g,"")}
		String.prototype.inStr=function(s1,s2){s2=s2||",";return (s2+this+s2).indexOf(s2+s1+s2)!=-1?true:false}
//				if(document.all)
//					document.onreadystatechange=function(){if(document.readyState=="complete")document_onload()}
//				else{
		if(browser.firefox){
			window.constructor.prototype.__defineGetter__("event",function(){var o=arguments.callee.caller;var e;while(o!=null){e=o.arguments[0];if(e&&(e.constructor==Event||e.constructor==MouseEvent))return e;o=o.caller;}return null;});
			Event.prototype.__defineGetter__("srcElement",function(){var node=this.target;while(node.nodeType!=1){node=node.parentNode}return node})
			Event.prototype.__defineGetter__("offsetX",function(){return this.layerX})
			Event.prototype.__defineGetter__("offsetY",function(){return this.layerY})
			Event.prototype.__defineSetter__("cancelBubble",function(b){if(b)this.stopPropagation();return b});
			//document.addEventListener("DOMContentLoaded",document_onload,false);
		}
	}
	window.setHtml=function(html){
		ow("eEditor").document.body.innerHTML="";
		editor_paste(html);
	}
	window.insertHtml=function(html){
		try{setFocus()}
		catch(e){}
		finally{editor_paste(html)}
	}
	window.setFocus=function(){ow("eEditor").focus()}
	function loadEvents(){
		if(document.all)
			$("eEditor").style.width=$("eEditor").style.height="100%";
		$("frmEditor").style.width=(docWidth()-(browser.msie?4:2))+"px";
		$("frmEditor").style.height=(docHeight()-(browser.msie?52:26))+"px";
		if(!document.all)
			setDesign("eEditor");
		else
			setDesign("fmBuf");
		if(ow("eEditor")){
			editor=ow("eEditor");
			editor.rge=null;
			editor.document.onkeydown=function(e){
				e=e||editor.event;
				if(e.keyCode==9&&browser.msie){
					editor.document.selection.createRange().pasteHTML("����");
					return false;
				}
				if(e.ctrlKey&&e.keyCode==13&&opForm){
					setOpText();
					opForm.click();
					return false;
				}
			}
			editor.document.onmousedown=function(){
				$("popup").hide();
			}
			editor.document.body.onmouseup=editor.document.body.onkeyup=editor.document.body.onfocus=function(){
				editor.rge=editor.document.selection.createRange();
			}
			editor.document.body.onpaste=function(){
				var html,w=ow("fmBuf");
				w.document.execCommand("selectall");
				w.document.execCommand("paste");
				html=w.document.body.innerHTML.replace(/\<[^\>]+\>/g,function($0){
					return /^<(img|a|font|br>|p|\/p>|\/a>|\/font>)/i.test($0)?$0:""
				})
				editor_paste(html);
				return false;
			}
		}
	}
	function splitUrl(type){
		var s=unescape(location.search).slice(1).split("&");
		for(var i=0;i<s.length;i++){
			a=s[i].split("=");
			if(a[0]==type)
				return a[1];
		}
		return null;
	}
	function document_onload(){
		var toolStr='<root>',toolbar,str=splitUrl("btn");
		window.opText=parent.document.getElementById(splitUrl("target")||"body");
		window.opForm=parent.document.getElementById("editorFormBtn");
		str=str||"fontname,fontsize,bold,italic,left,center,right,removeformat,link,color,bgcolor,image,face";
		initTool(str);
		$("popup").colors="000000,993300,333300,003300,003366,000080,333399,333333,800000,FF6600,808000,008000,008080,0000FF,666699,808080,FF0000,FF9900,99CC00,339966,33CCCC,3366FF,800080,969696,FF00FF,FFCC00,FFFF00,00FF00,00FFFF,00CCFF,993366,C0C0C0,FF99CC,FFCC99,FFFF99,CCFFCC,CCFFFF,99CCFF,CC99FF,FFFFFF";
		$("popup").faceTip="����,Ʋ��,ɫ,����,���,����,��ŭ,��Ƥ,��Ц,΢Ц,�ѹ�,��,����,��,͵Ц,��,��,�ɰ�,����,����,����ˮ,����,��,����,��,��,����,��,��Ц,���,˼��,����,��,��,ǿ,��,��,����,����,�ټ�,̾��,�ô�,�Է�,�绰,�׻�,����,��ͷ,è��,����,����,õ��,��л,����,����,����,����,ը��,����,���,ѩ��";
		$("popup").show=function(type,obj){
			if("color,bgcolor,face".inStr(type)){
				var x=obj.parentNode.offsetLeft,y=obj.parentNode.offsetTop+obj.parentNode.offsetHeight+1,img;
				if(browser.msie&&$("left"))
					x+=$("left").offsetWidth;
				if("color,bgcolor".inStr(type)){
					w=152;h=101;
					img="imgs/color_selector.gif";
				}
				else if(type=="face"){
					w=246;h=150;
					img="imgs/smiles.gif";
				}
				if((x+w)>docWidth())
					x=docWidth()-w-10;
				$("popup").setPos("popup",x,y,w,h);
				$("popup").setPos("faceImg",0,0,w,h);
				$("faceImg").type=type;
				$("faceImg").style.background="url("+img+") no-repeat";
				$("faceImg").onmousedown=function(){event.cancelBubble=true}
				$("faceImg").onmousemove=function(){$("popup").showShadow(type)}
				$("faceImg").onclick=function(){
					if(this.x==null||this.y==null)return;
					if(this.type=="face"){
						s='<img src="'+baseurl+'emot/emot-'+((this.x+this.y*10+1)+"").replace(/^(\d{1})$/g,"0$1")+'.gif" alt="'+$("popup").faceTip.split(",")[this.x+this.y*10]+'" />';
						editor_paste(s);
					}
					else if("color,bgcolor".inStr(type))
						exec(type=="color"?"forecolor":"backcolor","#"+$("popup").colors.split(",")[this.x+this.y*8]);
					$("popup").hide();
				}
				oShow("popup",1);
				oHide("shadow",1);
				oShow("faceImg",1);
			}
		}
		$("popup").hide=function(){
			oHide(this,1);
			oHide("shadow",1);
			oHide("faceImg",1);
			oHide("smallFace",1);
		}
		$("popup").setPos=function(obj,x,y,w,h){
			$(obj).style.left=x+"px";
			$(obj).style.top=y+"px";
			$(obj).style.width=w+"px";
			$(obj).style.height=h+"px";
		}
		$("popup").showShadow=function(type){
			if("color,bgcolor,face".inStr(type)){
				var px=event.offsetX,py=event.offsetY,offx=0,offy=0,w,h,allx,ally,pw,ph,x,y;
				if("color,bgcolor".inStr(type)){
					offx=3;offy=4;
					w=18;h=18;
					allx=7;ally=4;
					pw=16;ph=16;
				}
				else if(type=="face"){
					offx=offy=2;
					w=h=24;
					pw=ph=22;
					allx=9;ally=5;
				}
				px-=offx;
				py-=offy;
				x=Math.floor(px/w);y=Math.floor(py/h);
				if(x<0||y<0||x>allx||y>ally){
					oHide("shadow",1);
					oHide("smallFace",1);
					$("faceImg").x=$("faceImg").y=null;
					return;
				}
				$("faceImg").x=x;$("faceImg").y=y;
				this.setPos("shadow",(x*w+offx+1),(y*h+offy+1),pw,ph);
				if(type=="face"){
					$("smallFace").style.background="url(imgs/emot/emot-"+((x+y*10+1)+"").replace(/^(\d{1})$/g,"0$1")+".gif) #fff no-repeat center center";
					oShow("smallFace",1);
				}
				oShow("shadow",1);
			}
		}
		$("smallFace").selectDir=function(){
			if(this.offsetLeft<=10)
				this.style.left=(this.parentNode.offsetWidth-this.offsetWidth-2)+"px";
			else
				this.style.left="2px";
		}
		if(browser.msie)
			parent.document.attachEvent("onmousedown",function(){try{setOpText();$("popup").hide()}catch(e){}})
		else
			parent.document.addEventListener("mousedown",function(){try{setOpText();$("popup").hide()}catch(e){}},false)
		document.onmousedown=function(){try{$("popup").hide()}catch(e){}}
		document.onblur=document.onmouseout=function(){try{setOpText()}catch(e){}}
	}
	function setOpText(){
		if(opText){
			opText.value=getContent();
		}
	}
	function initTool(str){
		var fontname="����,����,����,����,��Բ,Arial,Arial Black,Courier,Tahoma,Verdana",a;
		var fontsize="Ĭ��,2;С,1;����,2;��,4;����,5;�ش�,6;����,7";
		if(str.inStr("fontname")){
			a=fontname.split(",");
			insHtml("left",'<select id="fontname" onchange="exec(\'fontname\',this.options[this.selectedIndex].value)"></select>');
			for(var i=0;i<a.length;i++)
				insHtml($("left").lastChild,'<option value="'+a[i]+'">'+a[i]+'</option>');
		}
		if(str.inStr("fontsize")){
			a=fontsize.split(";");
			insHtml("left",'<select id="fontsize" onchange="exec(\'fontsize\',this.options[this.selectedIndex].value)"></select>');
			for(var i=0;i<a.length;i++)
				insHtml($("left").lastChild,'<option value="'+a[i].split(",")[1]+'">'+a[i].split(",")[0]+'</option>');
		}
		if(str.inStr("bold"))
			createBtn("iconEditor_bold.gif","����",function(){exec("bold")});
		if(str.inStr("italic"))
			createBtn("iconEditor_italic.gif","б��",function(){exec("italic")});
		if(str.inStr("left"))
			createBtn("iconEditor_left.gif","�������",function(){exec("justifyleft")});
		if(str.inStr("center"))
			createBtn("iconEditor_center.gif","���ж���",function(){exec("justifycenter")});
		if(str.inStr("right"))
			createBtn("iconEditor_right.gif","���Ҷ���",function(){exec("justifyright")});
		if(str.inStr("removeformat"))
			createBtn("iconEditor_erase.gif","��Ƥ��",function(){exec("removeformat")});
		if(str.inStr("link"))
			createBtn("iconEditor_link.gif","���볬����",function(){special("createlink")});
		if(str.inStr("color"))
			createBtn("iconEditor_color.gif","������ɫ",function(){$("popup").show("color",this)},"color");
		if(str.inStr("bgcolor"))
			createBtn("iconEditor_bgcolor.gif","������ɫ",function(){$("popup").show("bgcolor",this)},"bgcolor");
		if(str.inStr("image"))
			createBtn("iconEditor_image.gif","����ͼƬ",function(){special("insertimage")});
		if(str.inStr("face"))
			createBtn("iconEditor_smile.gif","�������",function(){$("popup").show("face",this)},"face");
	}
	function setDesign(id){
		var o=ow(id);
		if(o&&o.document.designMode=="on")
			return;
		if(o)
			o.document.designMode="on";
		setTimeout("setDesign('"+id+"')",10);
	}
	function special(cmd){
		if("createlink,insertimage".inStr(cmd)){
			if(cmd=="createlink"){
				info="���볬������\n����Ϊhttp://��ftp://��ͷ";
				s1="(http|ftp)\:\/\/";
				s2="";
			}
			else{
				info="����ͼƬ����\n����Ϊhttp://��ͷ��֧�ָ�ʽjpg,gif,png,bmp";
				s1="http\:\/\/";
				s2="\.(jpg|gif|png|bmp)";
			}
			if(browser.msie)
				str=window.showModalDialog("input.htm",{"title":(cmd=="createlink"?"�����볬����·��<br />����Ϊhttp://��ftp://��ͷ":"������ͼƬ·��<br />����Ϊhttp://��ͷ��֧�ָ�ʽjpg,gif,png,bmp"),"value":"http://"},"dialogwidth:354px;dialogheight:152px;status:no;help:no")
			else
				str=prompt(info,"http://")
			if(str==null)return;
			str=checkJs(str,s1,s2);
			if(cmd=="createlink")
				exec(",http://,ftp://".inStr(str)?"unlink":"createlink",str);
			else{
				if(!",http://".inStr(str))
					editor_paste('<img src="'+str+'" alt="" />');
			}
		}
	}
	function editor_paste(html){
		var w=ow("eEditor");
		if(browser.msie){
			w.focus();
			w.range=w.rge||w.document.selection.createRange();
			w.range.select();
			try{
				w.range.pasteHTML(html);
				w.rge=w.range;
			}
			catch(e){w.document.body.insertAdjacentHTML("beforeend",html)}
		}
		else{
			var ol,i,nl=[],r=w.document.createRange();
			if(html=="")
				return;
			w.document.execCommand("insertimage",false,"http://temp_obj/");
			ol=w.document.getElementsByTagName("img");
			for(var i=0;i<ol.length;i++)
				nl.push(ol[i]);
			for(i=0;i<nl.length;i++){
				if(nl[i].src=="http://temp_obj/"){
					r.setStartBefore(nl[i]);
					nl[i].parentNode.insertBefore(r.createContextualFragment(html),nl[i]);
					nl[i].parentNode.removeChild(nl[i]);
				}
			}
		}
		w.focus();
	}
	function exec(cmd,val){
		var w=ow("eEditor"),info,str,s1,s2;
		if(browser.msie){
			w.range=w.rge||w.document.selection.createRange();
			w.range.select();
		}
		else{
			w.getSelection();
		}
		w.focus();
		w.document.execCommand(cmd,false,val);
	}
	function checkJs(str,cl,bl){
		var re=new RegExp("^("+cl+")(.+)?("+bl+")$"),s1;
		str=str.trim();
		s1=str.toLowerCase();
		return re.test(s1)?(/(.?)+(javascript(\s)*:)/g.test(s1)?"":str):"";
	}
	function getContent(){
		var html=ow("eEditor").document.body.innerHTML;
		html=dejs(html).replace(/(\n)*(\r)*/g,"");
		if(/^(\s)*(<br\>)*(\s)*$/i.test(html))
			html="";
		else if(/^\s*(<p>)*(&nbsp;)*\s*\n*\r*(<\/p>)*\s*$/i.test(html))
			html="";
		return html;
	}
	function isEmpty(){
		return getContent()==""?true:false;
	}
	function dejs(str){
		str=str.replace(/\<a /gi,"<a target=_blank ").replace(/href=(\'|\"|)javascript\:/gi,"xx=");
		str=str.replace(/\b(j\s*a\s*v\s*a\s*s\s*c\s*r\s*i\s*p\s*t)\b/gi,"");
		str=str.replace(/\<[^\>]+\>/g,function($0){
			return /^\s*<(.*(src=).*\.do[\"|\'|\&|\?|]|object|embed|script|iframe|style|link|meta|title|body|bgsound|frame|textarea|text|button|input|(\/script>)|\/embed>|\/object>|\/style\>|\/iframe>|\/title>|\/body>|\/textarea>|\/text>|\/button>)/i.test($0)?"":$0.replace(/\b(position)\s*\:\s*(absolute|static|relative)\s*[;|]/gi,"");
		});
		return str;
	}
	function createBtn(img,title,fnClick,id){
		s='<li><img src="imgs/blank.gif" alt="'+title+'" style="background-image:url(imgs/'+img+');background-position:0px -44px;width:23px;height:22px;" title="'+title+'"'+(id==null?'':'id="'+id+'"')+' /></li>';
		insHtml("toolbar",s);
		oImg=$("toolbar").lastChild.firstChild;
		oImg.onmouseover=oImg.onmouseup=function(){this.style.backgroundPosition="0px 0px"}
		oImg.onmousedown=function(){this.style.backgroundPosition="0px -22px"}
		oImg.onmouseout=function(){this.style.backgroundPosition="0px -44px"}
		oImg.onclick=fnClick;
	}
	function fillHtml(id,str){
		var oBx=$(id),oName=oBx.tagName.toLowerCase();
		if(oName=="select"){
			while(oBx.options.length>0)
				oBx.removeChild(oBx.options[0])
		}
		else if("table,tbody".inStr(oName)){
			if(oName=="table"&&oBx.tBodies.length==0)
				oBx.appendChild(document.createElement("tbody"));
			oBx.replaceChild(document.createElement("tbody"),oBx.tBodies[0]);
		}
		else
			oBx.innerHTML="";
		insHtml(oBx,str);
	}
	function insHtml(id,str,oRefer){
		var oDiv=document.createElement("div"),oBx=tBx=$(id),oRefer=oRefer==null?null:oRefer,oName=oBx.tagName.toLowerCase(),ol;
		id=typeof(id)=="string"?id:id.id;
		if("table,tbody".inStr(oName)){
			oDiv.innerHTML="<table><tbody>"+str+"</tbody></table>";
			ol=oDiv.childNodes[0].tBodies[0].rows;
			if(oName=="table"){
				if(oBx.tBodies[0].length==0)
					oBx.appendChild(document.createElement("tbody"));
				tBx=oBx.tBodies[0];
			}
		}
		else if(oName=="select"){
			oDiv.innerHTML="<select>"+str+"</select>";
			ol=oDiv.childNodes[0].options;
		}
		else if("div,span,body,label,ul,li".inStr(oName)){
			oDiv.innerHTML=str;
			ol=oDiv.childNodes;
		}
		while(ol.length>0)
			tBx.insertBefore(ol[0],oRefer);
		if(oName=="select")
			window.setTimeout("$('"+id+"').selectedIndex=0",10);
	}
