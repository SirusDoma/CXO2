-- O2Jam Live NX2 Content Version 1.0
-- DialogQuestion2
-- Copyright (C) 2016 - CXO2 a.k.a SirusDoma

local Background = ContentManager.Instance:GetTexture('Dialog/Question2/Background.ojs')

local DialogText = Text('Question2', ContentManager.Instance:GetFont('Common/Font.ttf'), 13)

local BtnYes = Button(ContentManager.Instance:GetTexture("Dialog/Question2/BtnYes.ojs"),
    Size(107, 26)
)
BtnYes.Transform.Position = Vector2(52, 181)

local BtnNo = Button(ContentManager.Instance:GetTexture("Dialog/Question2/BtnNo.ojs"),
    Size(107, 26)
)
BtnNo.Transform.Position = Vector2(159, 181)

local ObjTransform = Transform()
local DraggableArea = Rectangle(0, 0, 170, 40)

Elements:Add('Background', Background)
Elements:Add('DialogText', DialogText)
Elements:Add('BtnYes', BtnYes)
Elements:Add('BtnNo', BtnNo)
Elements:Add('Transform', ObjTransform)
Elements:Add('DraggableArea', DraggableArea)
