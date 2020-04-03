<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<base href="http://zhat.dk/test/" />
<title>Zhat.dk</title>
<link rel="stylesheet" type="text/css" href="../include/zhat.css" />
<style type="text/css">
<!--
#theRows tr { background-color: #ffffcc; }
#theRows tr.t0 { background-color: #ffffcc; }
#theRows tr.t1 { background-color: #ccccff; }
#theRows tr td { padding:2px; text-align: center; }
-->
</style>
</head>
<body>
<table cellspacing="0" class="outer">
<tr>
	<td colspan="2" valign="middle" class="heading"><img alt="Zhat zhat zhat..." src="../gfx/logo.gif" /></td>
</tr>
<tr valign="top">
	<td class="body" valign="top" style="width:95%;">
	<!-- Page header ends, content begins -->

	<h2>Template engine test page</h2>
	
	<p class="content">
		This page is an example of how to print stuff like search results using a template. 
		<br />All rows have the same formatting, but contain different data.
		In this example @@ROWS@@ rows are generated.
	</p>

	<table id="theRows" cellspacing="0" cellpadding="0" style="width:98%; border: 2px solid darkred; margin: 10px; background-color: white;">
	<tr class="dhead">
		<th>Column 1</th>
		<th>Column 2</th>
		<th>Column 3</th>
		<th>Column 4</th>
		<th>Column 5</th>
		<th>Column 6</th>
		<th>Column 7</th>
	</tr>
	<!-- @@ROW@@ -->
	<tr class="t@@TYPE@@">
		<td><b>@@FIELD1@@ of @@ROWS@@</b></td>
		<td>~(@@FIELD2@@)~</td>		
		<td>-(@@FIELD3@@)-</td>	
		<td>=(@@FIELD4@@)=</td>	
		<td>-=@@FIELD5@@=-</td>	
		<td>~=(@@FIELD6@@)=~</td>	
		<td>~-=(@@FIELD7@@)=-~</td>	
	</tr>
	<!-- @@ROW@@ -->
	</table>
	
	<!-- Content ends, page footer begins -->
	</td>
	<td class="copy" align="right" valign="bottom"><a href="mailto:Haj@zhat.dk"><img alt="&copy; Copyrighted by Niels Andersen" src="../gfx/copy.gif" style="margin-top:10px" /></a></td>
</tr>
</table>
</body>
</html>