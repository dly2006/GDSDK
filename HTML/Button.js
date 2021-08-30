window.onload = function () {

    "object" == typeof ue && "object" == typeof ue.interface

	/*config*/
	document.getElementById("GDSDK-Config").addEventListener("click", function () {//图层名称、图层地址
	   ue4("CONFIG", { "data": {"cname": "Config","curl": "http://10.1.15.87:8090/iserver/services/3D-SCsplijz/rest/realspace"} });
	});
    
	/* guodi sdk */
	document.getElementById("GDSDK-LayerManager").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"LayerManager": "图层管理"} });
	});
	document.getElementById("GDSDK-AddLayer").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"AddLayer": "添加图层"} });
	});
	document.getElementById("GDSDK-ModelAttribute").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"ModelAttribute": "场景属性"} });
	});
	document.getElementById("GDSDK-SceneClip").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"SceneClip": "场景裁剪"} });
	});
	document.getElementById("GDSDK-SpatialAnalysis").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"SpatialAnalysis": "通视分析"} });
	});
	document.getElementById("GDSDK-ViewShed").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"ViewShed": "可视域分析"} });
	});
	document.getElementById("GDSDK-Skyline").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"Skyline": "天际线分析"} });
	});
	document.getElementById("GDSDK-MeasureHandler").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"MeasureHandler": "量算"} });
	});
	document.getElementById("GDSDK-Position").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"Position": "坐标查询"} });
	});
	document.getElementById("GDSDK-AnalyzeTool").addEventListener("click", function () {
	   ue4("GDSDK", { "data": {"AnalyzeTool": "色彩分析"} });
	});
    
	/*demo*/
	document.getElementById("button1").addEventListener("click", function () {
        ue4("SetVisiblity", { "isvisible": true });
    });
    document.getElementById("button2").addEventListener("click", function () {
        ue4("SetVisiblity", { "isvisible": false });
    });
    document.getElementById("button3").addEventListener("click", function () {
        ue4("button", { "data": {"test": "按钮三按下了"} });
    });
    document.getElementById("button4").addEventListener("click", function () {
        ue4("button", { "data": {"test": "动态数值"} });
    });
    document.getElementById("button5").addEventListener("click", function () {
        ue4("button", { "data": {"test": "按钮五设置"} });
    });
	document.getElementById("button7").addEventListener("click", function () {
       window.location.reload()
    });
	document.getElementById("button8").addEventListener("click", function () {
        ue4("SetVisiblity", { "isvisible": false });
    });
	document.getElementById("button9").addEventListener("click", function () {
       ue4("button", { "data": {"test": "无刷新加载"} });
    });
	
	ue.interface.SetTitle = function (title) {
        document.getElementById("title").innerHTML = title
    }
};

