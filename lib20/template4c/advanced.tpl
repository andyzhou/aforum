<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<base href="http://zhat.dk/template/" />
<title>Zhat.dk</title>
<link rel="stylesheet" type="text/css" href="../include/zhat.css" />
<style type="text/css">
<!--
table.questions { width:95%; border: 1px solid darkred; margin: 10px; background-color: darkred; margin-bottom: 20px; }
table.questions tr { background-color: #ccccff; }
table.questions tr.t0 { background-color: #ffffcc; }
table.questions tr.t1 { background-color: #ccccff; }
table.questions th { padding:2px; text-align: center; }
table.questions td { padding:2px; padding-left: 5px; }
table.questions td.rt { width: 75px; text-align: right; padding:2px; }
table.questions td.footer { text-align: center; padding:2px; background-color: silver;}
-->
</style>
</head>
<body>
<table cellspacing="0" class="outer">
<tr>
	<td colspan="2" valign="middle" class="heading"><img alt="Zhat zhat zhat..." src="../gfx/logo.gif" /></td>
</tr>
<tr valign="top">
	<td class="body" valign="top" id="content" style="width:95%">
	<!-- Page header ends, content begins -->

	<h2>Template engine test page</h2>
	
	<p class="content">
		This example demonstrates how to generate a multiple choice test form using a template. 
		<br />The test is divided into @@CATEGORIES@@ categories, with @@QUESTIONS@@ questions each. 
		Each question can be answered in @@OPTIONS@@ ways.
		<br />
		<br />Off course, this is a simple example, so the values are fixed, 
		but if the form was to be generated using data from a database, the number of questions in each category,
		and the number of options pr. question, would probably be different for every category and every question.
		The template operations would however, be the same.
	</p>

	<form method="post" action="nothing" onsubmit="return false" style="margin:0; padding:0;">
	<!-- @@CATEGORY@@ -->
		<table class="questions" cellspacing="1" cellpadding="0">
		<tr>
			<th colspan="2" class="dhead">Test category number @@CATEGORY_NR@@</th>
		</tr>
		<!-- @@QUESTION@@ -->
		<tr class="t@@TYPE@@">
			<td>Question #@@QUESTION_NR@@</td>
			<td class="rt">
				<select name="a@@QUESTION_NR@@">
					<!-- @@OPTION@@ -->
					<option value="@@OPTION_VAL@@">@@OPTION_VAL@@</option>
					<!-- @@OPTION@@ -->
				</select>
			</td>		
		</tr>
		<!-- @@QUESTION@@ -->
		<tr>
			<td colspan="2" class="footer">Total: @@QUESTIONS@@ questions.</td>
		</tr>
		</table>
	<!-- @@CATEGORY@@ -->
	</form>
	<!-- Content ends, page footer begins -->
	</td>
	<td class="copy" align="right" valign="bottom"><a href="mailto:Haj@zhat.dk"><img alt="&copy; Copyrighted by Niels Andersen" src="../gfx/copy.gif" style="margin-top:10px" /></a></td>
</tr>
</table>
</body>
</html>