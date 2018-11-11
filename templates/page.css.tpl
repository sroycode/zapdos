body {
    padding: 0;
    margin: 0;
}

ol, ul {
    list-style: none;
}

div, ul, li, a, section, nav,
h1, h2, h3, h4, h5, h6,
hr, input, textarea, footer, header {
    -moz-box-sizing: border-box;
    -webkit-box-sizing: border-box;
    box-sizing: border-box;
    margin: 0;
    padding: 0;
    font-family: sans-serif;
}

.wrapper {
    width: 100%;
    clear: both;
}

.wrapper:after {
    visibility: hidden;
    display: block;
    font-size: 0;
    content: " ";
    clear: both;
    height: 0;
}

.row {
    width: 100%;
    max-width: 1200px;
    clear: both;
    margin-left: auto;
    margin-right: auto;
    margin-top: 40px;
}

.col {
    float: left;
}

.col + .col {
    padding-left: 40px;
}

.half {
    width: 50%;
}

.third {
    width: 33.33%;
}

.two-third {
    width: 66.66%
}

.quarter {
    width: 25%;
}

.wide {
    width: 100%;
    padding-right: inherit;
}

.mshow, .tshow {
    display: none;
}

footer {
    height: 100px;
    line-height: 100px;
    text-align: center;
}

header {
    color: #212121;
}

footer {
    background-color: #212121;
    color: #FDFDFD;
    margin-top: 40px;
}

footer a {
    color: #FDFDFD;
}

.branding {
    float: left;
    margin: 10px 0 0 10px;
}

header .get-in-touch {
    float: right;
    width: 200px;
    line-height: 50px;
    height: 50px;
    margin-right: 10px;
    background-color: #ABBCC7;
}

input[type="text"], input[type="password"], input[type="date"],
input[type="datetime"], input[type="email"], input[type="number"],
input[type="search"], input[type="tel"], input[type="time"],
input[type="url"], textarea {
    background-color: white;
    border: 1px solid #CCCCCC;
    border-radius: 2px 2px 2px 2px;
    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.1) inset;
    color: rgba(0, 0, 0, 0.75);
    display: block;
    font-family: inherit;
    font-size: 14px;
    height: 56px;
    margin: 0 0 14px;
    padding: 7px;
    width: 100%;
}

input[type="submit"],
.button {
    border: none;
    border-radius: 2px;
    padding: 0 20px;
    text-decoration: none;
    color: #fff;
    font-size: 1em;
    background-color: #E0B3A2;
    display: inline-block;
    margin-top: 20px;
    height: 40px;
    line-height: 40px;
    text-align: center;
    cursor: pointer;
}

input[type="submit"]:hover,
.button:hover {
    background-color: #666;
    color: #fff;
}

.map {
    width: 100%;
    height: 400px;
}

.photon-autocomplete li.on {
    background-color: #e0b3a2;
}

h2 {
    min-height: 70px;
    line-height: 30px;
    text-align: center;
}

.map-wrapper {
    background-color: #6A8090;
    color: #eee;
}

.map-wrapper h2 {
    text-align: left;
    color: #eee;
}

pre {
    background-color: #f5f5f5;
    border: 1px solid #eee;
    color: #444;
    padding: 10px;
}

pre strong {
    color: #666;
}

.highlights {
    margin-bottom: 40px;
}

.highlights span.url {
	color: #999;
	font-size: 0.9em;
	margin-top: 10px;
	display: inline-block;
}

.highlights img {
    vertical-align: top;
    height: 64px;
}

.highlights .col.third {
    text-align: center;
}

.highlights a {
    text-decoration: none;
    color: #666;
}

.highlights a:hover {
    color: #000;
}

.highlights a:hover span.url {
    color: #666;
}

.api {
    background-color: #AECDBF;
    color: #f1f1f1;
}

.wrapper .content {
    padding: 40px;
}

.api .content ul {
    background-color: #fff;
    padding: 20px;
}

.api h4 {
    color: #333;
}

@media only screen and (max-width: 770px) {
    .row {
        padding-left: 5px;
        padding-right: 5px;
    }

    .twide {
        width: 100%;
        padding-left: 0 !important;
    }

    .tthird {
        width: 33.3333%;
    }

    .tshow {
        display: inherit;
    }

    .thide {
        display: none;
    }

    .wrapper .content {
        padding: 0;
    }
}

@media only screen and (max-width: 639px) {
    .mwide {
        padding-left: 0 !important;
        width: 100%;
    }

    .mwide + .mwide {
        margin-top: 20px;
    }

    .mthird {
        width: 33.3333%;
    }

    .mshow {
        display: inherit;
    }

    .mhide {
        display: none;
    }
}

ul.features li {
    line-height: 3em;
}

ul li span.progress {
    background: #eee;
    color: #333;
    font-size: 0.75em;
    padding: 2px 4px;
    border-radius: 5px;
}

header {
	padding: 20px;
}

#slogan {
	font-size: 1.4em;
	color: #999;
	margin-top: 10px;
}

#logo {
	float: left;
	height: 110px;
	margin-right: 35px;
	margin-left: 10px;
}

#photon {
	font-size: 3.4em;
	font-style: italic;
	display: inline-block;
	padding-top: 12px;
}
