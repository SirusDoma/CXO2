-- O2Jam Live NX2 Content Version 1.0
-- DialogQuestion1
-- Copyright (C) 2016 - CXO2 a.k.a SirusDoma

local Background = ContentManager.Instance:GetTexture('Dialog/Question1/Background.ojs')

local DialogText = Text('Question2', ContentManager.Instance:GetFont('Common/Font.ttf'), 13)

local BtnOK = Button(ContentManager.Instance:GetTexture("Dialog/Question1/BtnOK.ojs"),
    Size(107, 26)
)
BtnOK.Transform.Position = Vector2(52, 181)

local BtnCancel = Button(ContentManager.Instance:GetTexture("Dialog/Question1/BtnCancel.ojs"),
    Size(107, 26)
)
BtnCancel.Transform.Position = Vector2(159, 181)

local ObjTransform = Transform()
local DraggableArea = Rectangle(0, 0, 170, 40)

Elements:Add('Background', Background)
Elements:Add('DialogText', DialogText)
Elements:Add('BtnOK', BtnOK)
Elements:Add('BtnCancel', BtnCancel)
Elements:Add('Transform', ObjTransform)
Elements:Add('DraggableArea', DraggableArea)
