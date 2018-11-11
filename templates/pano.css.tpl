ul.pano-autocomplete {
    position: absolute;
    background-color: white;
    z-index: 1000;
    box-shadow: 0 4px 9px #999999;
    display: none;
}
.pano-autocomplete li {
    min-height: 40px;
    line-height: 1em;
    padding: 5px 10px;
    overflow: hidden;
    white-space: nowrap;
    font-size: 1em;
}
.pano-autocomplete li strong {
    display: block;
}
.pano-autocomplete li.on {
    background-color: #2980b9;
    cursor: pointer;
}
.pano-autocomplete li.pano-no-result {
    text-align: center;
    color: #666;
    font-size: 0.9em;
    line-height: 40px;
}
.pano-autocomplete .pano-feedback {
    display: block;
    text-align: right;
    font-size: 0.8em;
    padding: 3px;
    color: #999;
    border-top: 1px solid #eee;
}
.pano-input:focus {
    width: 400px;
}
