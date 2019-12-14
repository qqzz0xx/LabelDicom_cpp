#pragma once

enum MprType
{
	Sagittal = 0,
	Coronal,
	Axial,
	None,
	RandomX,
	RandomY,
	RandomZ,
};

enum ImageFileType
{
	ImageFileType_None,
	ImageFileType_NiTFI,
	ImageFileType_MetaImage,
	ImageFileType_VolumeDicom,
	ImageFileType_SingleFrame,
	ImageFileType_SequenceFrame,
	ImageFileType_MaskData,
};

enum ImageShowType
{
	ImageShowType_Image = 0,
	ImageShowType_Video,
	ImageShowType_Volume,
};

enum MouseEventType
{
	MouseEventType_Move,
	MouseEventType_LeftDown,
	MouseEventType_LeftUp,
	MouseEventType_RightDown,
	MouseEventType_RightUp,
	MouseEventType_MiddleDown,
	MouseEventType_MiddleUp,
	MouseEventType_WheelForward,
	MouseEventType_WheelBackward,
	MouseEventType_KeyPress,
	MouseEventType_KeyRelease,
};

