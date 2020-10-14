<html>
<head><title>Problem Report ports/48515: ports chown</title>
<meta name="robots" content="nofollow">
</head>
<body text="#000000" bgcolor="#ffffff">
<IMG SRC="../gifs/bar.gif" ALT="Navigation Bar" WIDTH="565" HEIGHT="33" BORDER=0 usemap="#bar">
<map name="bar">
<area shape="rect" coords="1,1,111,31" href="../index.html" ALT="Top">
<area shape="rect" coords="112,11,196,31" href="../ports/index.html" ALT="Applications">
<area shape="rect" coords="196,12,257,33" href="../support.html" ALT="Support">
<area shape="rect" coords="256,12,365,33" href="../docs.html" ALT="Documentation"> 
<area shape="rect" coords="366,13,424,32" href="../commercial/commercial.html" ALT="Vendors">
<area shape="rect" coords="425,16,475,32" href="../search/search.html" ALT="Search">
<area shape="rect" coords="477,16,516,33" href="../search/index-site.html" ALT="Index">
<area shape="rect" coords="516,15,562,33" href="../index.html" ALT="Top">
<area shape="rect" href="../index.html" coords="0,0,564,32" ALT="Top">
</map><h1><font color="#660000">Problem Report ports/48515</font></h1>
<strong>ports chown</strong><p>
<dl>

<dt><strong>Confidential</strong><dd>
no
<dt><strong>Severity</strong><dd>
serious
<dt><strong>Priority</strong><dd>
high
<dt><strong>Responsible</strong><dd>
<a href="mailto:leeym@FreeBSD.org">leeym@FreeBSD.org</a>
<dt><strong>State</strong><dd>
closed
<dt><strong>Class</strong><dd>
change-request
<dt><strong>Submitter-Id</strong><dd>
current-users
<dt><strong>Arrival-Date</strong><dd>
Thu Feb 20 21:30:05 PST 2003
<dt><strong>Closed-Date</strong><dd>
Sun Feb 23 08:26:03 PST 2003
<dt><strong>Last-Modified</strong><dd>
Sun Feb 23 08:26:03 PST 2003
<dt><strong>Originator</strong><dd>
Akihiro IIJIMA &lt;<A HREF="mailto:aki@bsdclub.org">aki@bsdclub.org</A>&gt;
<dt><strong>Release</strong><dd>
FreeBSD 5.0-RELEASE-p1 i386
<dt><strong>Description</strong><dd>

<pre>
chown error at ports
syntax change  (chown user.group -&gt; chown user:group)

games/lexter/pkg-install:chown root.games $FILE
games/nighthawk/pkg-install:chown root.games $FILE
japanese/Wnn6/pkg-install:      chown bin.bin ${startup_script}
mail/sentinel/pkg-plist:@exec chown smmsp.smmsp %D/var/sentinel

</pre>
<dt><strong>Fix</strong><dd>

<pre>
	change "chown ???:???"

</pre>
<dt><strong>Audit-Trail</strong><dd>

<pre>
Responsible-Changed-From-To: freebsd-ports-bugs-&gt;leeym 
Responsible-Changed-By: leeym 
Responsible-Changed-When: Sat Feb 22 08:22:10 PST 2003 
Responsible-Changed-Why:  
I will handle this pr. 

<A HREF="http://www.freebsd.org/cgi/query-pr.cgi?pr=48515">http://www.freebsd.org/cgi/query-pr.cgi?pr=48515</A> 
State-Changed-From-To: open-&gt;closed 
State-Changed-By: leeym 
State-Changed-When: Sun Feb 23 08:26:01 PST 2003 
State-Changed-Why:  
Committed, thanks. 

<A HREF="http://www.freebsd.org/cgi/query-pr.cgi?pr=48515">http://www.freebsd.org/cgi/query-pr.cgi?pr=48515</A> 
</pre>
</dl><A HREF="mailto:freebsd-gnats-submit@FreeBSD.org,aki@bsdclub.org?subject=Re:%20ports/48515:%20ports%20chown">Submit Followup</A> | <A HREF="./query-pr.cgi?pr=48515&amp;f=raw">Raw PR</A>
<hr><address><a href="../mailto.html">www@FreeBSD.org</a><br></address>
