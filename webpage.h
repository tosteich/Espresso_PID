const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
   <link rel="manifest" href="https://tosteich.github.io/projectsForFun/manifest.json">
   <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta1/dist/css/bootstrap.min.css" rel="stylesheet"
    integrity="sha384-giJF6kkoqNQ00vy+HMDP7azOuL0xtbfIcaT9wjKHr8RbDVddVHyTfAAsrekwKmP1" crossorigin="anonymous">
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta1/dist/js/bootstrap.bundle.min.js"
    integrity="sha384-ygbV9kiqUc6oa4msXn9868pTtWMgiQaeYH7/t7LECLbyPA2x65Kgf80OJFdroafW"
    crossorigin="anonymous"></script>
  <style>
    html {
      display: inline-block;
      margin: 0px auto;
    }

    body {
      font-family: Arial;
    }

    h2 {
      font-size: 3.0rem;
      text-align: center;
      margin-block: 0;
    }

    h1 {
      font-size: 2.0rem;
      text-align: center;
      margin-block: 0;
    }

    .one-row {
      font-size: 2.5rem;
      margin: auto;
      display: flex;
      flex-wrap: wrap;
      max-width: 600px;
      min-width: 346px;
      padding: 4px 4px;
    }

    .ds-labels {
      font-size: 1.4rem;
      vertical-align: middle;
      min-width: 170px;
      padding: 10px 0 0 8px;
      display: flex;
    }

    .value {
      text-align: right;
      min-width: 90px;
      max-width: 150px;
      font-size: 2.5rem;
      display: block;
      margin-left: auto;
      padding: 0;
    }

    .icon {
      min-width: 40px;
      text-align: center;
    }

    .timerButton {
      display: flex;
    }

    #temps {
      width: 80px;
    }

    #indicator {
      max-width: 360px;
      margin-left: auto;
      text-align: right;
      align-self: center;
    }

    .red i {
      color: crimson;
    }

    .green i {
      color: lawngreen;
    }

    .conf-text {
      color: #059e8a;
      animation-name: confirm;
      animation-duration: 4s;
    }

    @keyframes confirm {
      from {
        color: lawngreen;
      }

      to {
        color: black;
      }
    }

    .tab-content {
      width: 100%;
    }

    .nav-link {
      font-size: 1.5rem;
      color: black;
    }

    .highcharts-figure,
    .highcharts-data-table table {
      width: 100%;
      margin: 4px auto;
    }

    .narrow-left {
      width: 50%;
    }
    .narrow-right {
      width: 50%;
    }
    #container {
      height: 75vh;
    }
    #containerPressure,
    #pressGauge,
    #tempGauge {
      height: 260px;
    }
    #pressGauge,
    #tempGauge,
    #timerGauge {
      height: 220px;
    }
    .highcharts-data-table table {
      font-family: Verdana, sans-serif;
      border-collapse: collapse;
      border: 1px solid #EBEBEB;
      margin: 10px auto;
      text-align: center;
      width: 100%;
      max-width: 500px;
    }
    .highcharts-data-table caption {
      padding: 1em 0;
      font-size: 1.2em;
      color: #555;
    }
    .highcharts-data-table th {
      font-weight: 600;
      padding: 0.5em;
    }
    .highcharts-data-table td,
    .highcharts-data-table th,
    .highcharts-data-table caption {
      padding: 0.5em;
    }
    .highcharts-data-table thead tr,
    .highcharts-data-table tr:nth-child(even) {
      background: #f8f8f8;
    }
    .highcharts-data-table tr:hover {
      background: #f1f7ff;
    }
    .gaugeLabel {
      opacity: 1!important;
    }

  </style>
</head>
<body>
  <div class="one-row">
    <h2>GAGGIA</h2>
    <button id="indicator" class="btn btn-outline-secondary red" onclick="reboot()">Reboot <i
        class="fas fa-bullseye"></i></button>
  </div>
  <div class="container">
    <ul class="nav nav-tabs nav-fill" id="pills-tab" role="tablist" style="width:100%">
      <li class="nav-item" role="presentation">
        <a class="nav-link active" id="pills-home-tab" data-bs-toggle="pill" href="#pills-home" role="tab"
          aria-controls="pills-home" aria-selected="true">Home</a>
      </li>
      <li class="nav-item" role="presentation">
        <a class="nav-link" id="pills-graphs-tab" data-bs-toggle="pill" href="#pills-graphs" role="tab"
          aria-controls="pills-graphs" aria-selected="false">Graphs</a>
      </li>
      <li class="nav-item" role="presentation">
        <a class="nav-link" id="pills-profile-tab" data-bs-toggle="pill" href="#pills-profile" role="tab"
          aria-controls="pills-profile" aria-selected="false">Settings</a>
      </li>
    </ul>
  </div>
    <div class="tab-content" id="pills-tabContent">
      <div class="tab-pane fade show active" id="pills-home" role="tabpanel" aria-labelledby="pills-home-tab">
        <h1 id="currentTime">standby 02:20</h1>
        <div class="one-row">
          <figure class="highcharts-figure narrow-left">
            <div id="tempGauge"></div>
          </figure>
          <figure class="highcharts-figure narrow-right">
            <div id="pressGauge"></div>
          </figure>
        </div>
        <div class="one-row">
          <figure class="highcharts-figure">
            <div id="timerGauge"></div>
          </figure>
        </div>
      </div>
      <div class="tab-pane fade" id="pills-graphs" role="tabpanel" aria-labelledby="pills-graphs-tab">
        <div class="container">
          <figure class="highcharts-figure">
            <div id="container"></div>
          </figure>
        </div>
      </div>
      <div class="tab-pane fade" id="pills-profile" role="tabpanel" aria-labelledby="pills-profile-tab">
        <div class="one-row mt-2 mb-4">
          <a href="https://router.asus.com/RT-N16/ESD-USB/Gaggia/gaggia_2.html" class="btn btn-secondary">Graphs archive</a>
        </div>
        <div class="one-row">
          <div class="ds-labels">Change target Temp:</div>
          <input type="number" class="value form-control" id="temps" onchange="changeTemp();" min="20" max="105"
            value='00'>
        </div>
        <div class="one-row">
          <div class="ds-labels">Change Pid Kp:</div>
          <input type="number" class="value form-control" id="pidKp" step="0.01" onchange="changePidKp()" value='0.00'>
        </div>
        <div class="one-row">
          <div class="ds-labels">Change Pid Ki:</div>
          <input type="number" class="value form-control" id="pidKi" step="0.01" onchange="changePidKi()" value='0.00'>
        </div>
        <div class="one-row">
          <div class="ds-labels">Change Pid Kd:</div>
          <input type="number" class="value form-control" id="pidKd" step="0.01" onchange="changePidKd()" value='0.00'>
        </div>
      </div>
    </div>
</body>
<script src="https://code.highcharts.com/highcharts.src.js"></script>
<script src="https://code.highcharts.com/highcharts-more.js"></script>
<script type="text/javascript" src="https://tosteich.github.io/projectsForFun/appScript.js"></script>
</html>)rawliteral";
