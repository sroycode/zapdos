<!doctype html>
<title>Help Section</title>
<style>
body { text-align: left; }
h1 { font-size: 16px; }
h2 { color: #696969; text-decoration: none; font-size: 15px;}
body { font: 14px Helvetica, sans-serif; }
article { display: block; text-align: left; margin: 1 auto; }
ul { list-style-type: square;}
</style>

<h1>Help Section</h1>
Lists the basic api available

{{#ONE_RESULT}}
<article>
<h2>{{ROUTE}}</h2>
<ul>
{{#DESC_SECTION}}
<li>{{DESC}}</li>
{{/DESC_SECTION}}
</ul></p>
</article>
{{/ONE_RESULT}}
