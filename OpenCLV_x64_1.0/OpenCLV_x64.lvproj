<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="12008004">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="NI.SortType" Type="Int">3</Property>
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="User Controls" Type="Folder">
			<Item Name="Offset.ctl" Type="VI" URL="../User Controls/Offset.ctl"/>
			<Item Name="Array Dimensions.ctl" Type="VI" URL="../User Controls/Array Dimensions.ctl"/>
			<Item Name="Kernel Object.ctl" Type="VI" URL="../User Controls/Kernel Object.ctl"/>
			<Item Name="Copy Parameters.ctl" Type="VI" URL="../User Controls/Copy Parameters.ctl"/>
		</Item>
		<Item Name="Device Manipulation" Type="Folder">
			<Item Name="Error Handler.vi" Type="VI" URL="../Device Manipulation/Error Handler.vi"/>
			<Item Name="Create Device GUI.vi" Type="VI" URL="../Device Manipulation/Create Device GUI.vi"/>
			<Item Name="Delete Device.vi" Type="VI" URL="../Device Manipulation/Delete Device.vi"/>
			<Item Name="Start OpenCLV.vi" Type="VI" URL="../Device Manipulation/Start OpenCLV.vi"/>
			<Item Name="Stop OpenCLV.vi" Type="VI" URL="../Device Manipulation/Stop OpenCLV.vi"/>
		</Item>
		<Item Name="Functions" Type="Folder">
			<Item Name="Anisotropic Filter" Type="Folder">
				<Item Name="Anisotropic Filter 32f.vi" Type="VI" URL="../Functions/Anisotropic Filter/Anisotropic Filter 32f.vi"/>
				<Item Name="SRAD Filter 32f.vi" Type="VI" URL="../Functions/Anisotropic Filter/SRAD Filter 32f.vi"/>
			</Item>
			<Item Name="Convolution" Type="Folder">
				<Item Name="Convolution 32f.vi" Type="VI" URL="../Functions/Convolution/Convolution 32f.vi"/>
			</Item>
			<Item Name="Differential" Type="Folder">
				<Item Name="Gradient 32f.vi" Type="VI" URL="../Functions/Differential/Gradient 32f.vi"/>
				<Item Name="Gradient 64f.vi" Type="VI" URL="../Functions/Differential/Gradient 64f.vi"/>
				<Item Name="Laplacian 32f.vi" Type="VI" URL="../Functions/Differential/Laplacian 32f.vi"/>
				<Item Name="Laplacian 64f.vi" Type="VI" URL="../Functions/Differential/Laplacian 64f.vi"/>
			</Item>
			<Item Name="FFT" Type="Folder">
				<Item Name="FFT 32f.vi" Type="VI" URL="../Functions/FFT/FFT 32f.vi"/>
				<Item Name="FFT 64f.vi" Type="VI" URL="../Functions/FFT/FFT 64f.vi"/>
			</Item>
		</Item>
		<Item Name="Memory Manipulation" Type="Folder">
			<Item Name="Allocate Image.vi" Type="VI" URL="../Memory Manipulation/Allocate Image.vi"/>
			<Item Name="Allocate Memory.vi" Type="VI" URL="../Memory Manipulation/Allocate Memory.vi"/>
			<Item Name="Copy Memory.vi" Type="VI" URL="../Memory Manipulation/Copy Memory.vi"/>
			<Item Name="Delete Memory.vi" Type="VI" URL="../Memory Manipulation/Delete Memory.vi"/>
			<Item Name="Delete Image.vi" Type="VI" URL="../Memory Manipulation/Delete Image.vi"/>
			<Item Name="Read Image.vi" Type="VI" URL="../Memory Manipulation/Read Image.vi"/>
			<Item Name="Read Memory.vi" Type="VI" URL="../Memory Manipulation/Read Memory.vi"/>
			<Item Name="Write Image.vi" Type="VI" URL="../Memory Manipulation/Write Image.vi"/>
			<Item Name="Write Memory.vi" Type="VI" URL="../Memory Manipulation/Write Memory.vi"/>
			<Item Name="Register Memory or Image Event.vi" Type="VI" URL="../Memory Manipulation/Register Memory or Image Event.vi"/>
		</Item>
		<Item Name="Program Manipulation" Type="Folder">
			<Item Name="Load Program From String.vi" Type="VI" URL="../Program Manipulation/Load Program From String.vi"/>
			<Item Name="Delete Kernel.vi" Type="VI" URL="../Program Manipulation/Delete Kernel.vi"/>
			<Item Name="Delete Program.vi" Type="VI" URL="../Program Manipulation/Delete Program.vi"/>
			<Item Name="Execute Kernel Advanced.vi" Type="VI" URL="../Program Manipulation/Execute Kernel Advanced.vi"/>
			<Item Name="Execute Kernel Simple.vi" Type="VI" URL="../Program Manipulation/Execute Kernel Simple.vi"/>
			<Item Name="Load Kernel.vi" Type="VI" URL="../Program Manipulation/Load Kernel.vi"/>
			<Item Name="Register Kernel Event.vi" Type="VI" URL="../Program Manipulation/Register Kernel Event.vi"/>
			<Item Name="Load Program From File.vi" Type="VI" URL="../Program Manipulation/Load Program From File.vi"/>
			<Item Name="Set Kernel Arguments.vi" Type="VI" URL="../Program Manipulation/Set Kernel Arguments.vi"/>
			<Item Name="Kernel Workgroup Information.vi" Type="VI" URL="../Program Manipulation/Kernel Workgroup Information.vi"/>
			<Item Name="Basic Program Editor.vi" Type="VI" URL="../Program Manipulation/Basic Program Editor.vi"/>
		</Item>
		<Item Name="_Examples" Type="Folder" URL="../_Examples">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="_Help" Type="Folder" URL="../_Help">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="_EULA" Type="Folder">
			<Item Name="EULA.txt" Type="Document" URL="../_EULA/EULA.txt"/>
		</Item>
		<Item Name="OpenCLV_x64d.dll" Type="Document" URL="../OpenCLV_x64d.dll"/>
		<Item Name="clAmdFft.Runtime.dll" Type="Document" URL="../clAmdFft.Runtime.dll"/>
		<Item Name="OpenCLV_x64.dll" Type="Document" URL="../OpenCLV_x64.dll"/>
		<Item Name="OpenCLV x64.lvlib" Type="Library" URL="../OpenCLV x64.lvlib"/>
		<Item Name="OpenCLV Funtional Global Template.vit" Type="VI" URL="../OpenCLV Funtional Global Template.vit"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Check if File or Folder Exists.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Check if File or Folder Exists.vi"/>
				<Item Name="DialogType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogType.ctl"/>
				<Item Name="General Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler.vi"/>
				<Item Name="NI_AALPro.lvlib" Type="Library" URL="/&lt;vilib&gt;/Analysis/NI_AALPro.lvlib"/>
				<Item Name="Space Constant.vi" Type="VI" URL="/&lt;vilib&gt;/dlg_ctls.llb/Space Constant.vi"/>
				<Item Name="subFile Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/FileDialogBlock.llb/subFile Dialog.vi"/>
				<Item Name="Read PNG File.vi" Type="VI" URL="/&lt;vilib&gt;/picture/png.llb/Read PNG File.vi"/>
				<Item Name="Unflatten Pixmap.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pixmap.llb/Unflatten Pixmap.vi"/>
				<Item Name="NI_AALBase.lvlib" Type="Library" URL="/&lt;vilib&gt;/Analysis/NI_AALBase.lvlib"/>
				<Item Name="Read BMP File.vi" Type="VI" URL="/&lt;vilib&gt;/picture/bmp.llb/Read BMP File.vi"/>
				<Item Name="Get System Directory.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/sysdir.llb/Get System Directory.vi"/>
				<Item Name="System Directory Type.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/sysdir.llb/System Directory Type.ctl"/>
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="Open URL in Default Browser.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser.vi"/>
				<Item Name="Open URL in Default Browser (string).vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser (string).vi"/>
				<Item Name="Open URL in Default Browser (path).vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser (path).vi"/>
				<Item Name="Application Directory.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Application Directory.vi"/>
			</Item>
			<Item Name="lvanlys.dll" Type="Document" URL="/&lt;resource&gt;/lvanlys.dll"/>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
