/*
  Copyright (c) 2017 Alberto Otero de la Roza
  <aoterodelaroza@gmail.com>, Robin Myhr <x@example.com>, Isaac
  Visintainer <x@example.com>, Richard Greaves <x@example.com>, Ángel
  Martín Pendás <angel@fluor.quimica.uniovi.es> and Víctor Luaña
  <victor@fluor.quimica.uniovi.es>.

  critic2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at
  your option) any later version.

  critic2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "imgui_widgets.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm>

using namespace ImGui;

// Tooltip variables and behavior //
// Variables:
// - tooltip_shownid: ID for the tooltip element shown.
// - tooltip_hoveredid: ID for the tooltip element hovered last frame.
// - tooltip_thisframe: saves the current frame to detect when
//   AttachTooltip is called for the first time.
// - tooltip_washovered: set to true if any AttachTooltip element
//   has been hovered in the last frame.
// - tooltip_time: the last time when no item was hovered.
// - tooltip_lastactive - the last time when a tooltip was shown.
// How it works:
// The tooltip is shown if:
// - The mouse hovers over a single tooltip element for longer than
//   delay, which is passed as a function argument.
// - The mouse hovers over a tooltip element and it was already
//   showing a tooltip from a different element. In this case,
//   there is no delay.
// - The mouse hovers over a tooltip element, then hovers none for a
//   period of time less than delay, then overs over a new tooltip
//   element. In this case, the delay is calculated as the difference
//   between the time the mouse hovered over the new tooltip element
//   (tooltip_time) and the time the mouse left the previous tooltip
//   element (tooltip_lastactive), with a maximum equal to
//   delay. Thus, the delay is equal to the time the mouse spent
//   outside any tooltip elment.
static ImGuiID tooltip_shownid = 0;
static ImGuiID tooltip_hoveredid = 0;
static int tooltip_thisframe = -1;
static bool tooltip_washovered = false;
static float tooltip_time = 0.f;
static float tooltip_lastactive = 0.f;

// Function definitions //

void ImGui::SlidingBar(const char *label, ImGuiWindow* window, ImVec2 *pos, 
                       ImVec2 size, float minx, float maxx, int direction){
  ImDrawList* dl = window->DrawList;
  ImGuiContext *g = GetCurrentContext();
  bool hovered, held;
  const ImU32 color = GetColorU32(ImGuiCol_ResizeGrip);
  const ImU32 coloractive = GetColorU32(ImGuiCol_ResizeGripActive);
  const ImU32 colorhovered = GetColorU32(ImGuiCol_ResizeGripHovered);
  
  const ImRect slidingrect(*pos,*pos+size);
  const ImGuiID slidingid = window->GetID(label);
  ButtonBehavior(slidingrect, slidingid, &hovered, &held);

  if (held){
    if (direction == 1)
      pos->x = max(min(g->IO.MousePos.x - 0.5f * size.x,maxx),minx);
    else
      pos->y = max(min(g->IO.MousePos.y - 0.5f * size.y,maxx),minx);
  }

  // draw the rectangle
  dl->PushClipRectFullScreen();
  dl->AddRectFilled(slidingrect.GetTL(),slidingrect.GetBR(),
                    held?coloractive:(hovered?colorhovered:color),
                    g->Style.ScrollbarRounding);
  dl->PopClipRect();
}

bool ImGui::ButtonWithX(const char* label, const ImVec2& size, bool activetab, bool scrollbarcol,
                        bool *p_open, bool *dragged, bool *dclicked, float alphamul /*=1.f*/){
  // lengths and colors
  ImGuiContext *g = GetCurrentContext();
  const float crossz = round(0.3 * g->FontSize);
  const float crosswidth = 2 * crossz + 6;
  const float mintabwidth = 2 * crosswidth + 1;
  const ImU32 cross_color = GetColorU32(ImGuiCol_Text,alphamul);
  const ImU32 cross_color_hovered = GetColorU32(ImGuiCol_TextSelectedBg,alphamul);
  ImU32 color = GetColorU32(ImGuiCol_FrameBg,alphamul);
  ImU32 color_active = GetColorU32(ImGuiCol_FrameBgActive,alphamul);
  ImU32 color_hovered = GetColorU32(ImGuiCol_FrameBgHovered,alphamul);
  if (scrollbarcol){
    color = GetColorU32(ImGuiCol_ScrollbarGrab,alphamul);
    color_active = GetColorU32(ImGuiCol_ScrollbarGrabActive,alphamul);
    color_hovered = GetColorU32(ImGuiCol_ScrollbarGrabHovered,alphamul);
  }

  // size of the main button
  ImVec2 mainsize = size;
  if (p_open && size.x >= mintabwidth)
    mainsize.x -= crosswidth;

  // main button
  bool clicked = InvisibleButton(label, mainsize);

  // some positions and other variables
  bool hovered = IsItemHovered();
  ImVec2 pos0 = GetItemRectMin();
  ImVec2 pos1s = GetItemRectMax();

  // set the output flags for the main button
  *dragged = IsItemActive() && IsMouseDragging();
  *dclicked = IsItemActive() && IsMouseDoubleClicked(0);
  
  // draw the close button, if this window can be closed
  ImVec2 center;
  if (p_open && size.x >= mintabwidth){
    // draw the close button itself
    SameLine();
    char tmp2[strlen(label)+6];
    ImFormatString(tmp2,IM_ARRAYSIZE(tmp2),"%s__x__",label);
    ImVec2 smallsize(crosswidth, size.y);

    // cross button
    *p_open = !(InvisibleButton(tmp2, smallsize));

    // update output flags and variables for drawing
    *dragged = *dragged | (p_open && IsItemActive() && IsMouseDragging());
    hovered |= IsItemHovered();
    center = ((GetItemRectMin() + GetItemRectMax()) * 0.5f);
  }
  ImVec2 pos1 = GetItemRectMax();

  // rectangle and text
  ImDrawList* drawl = GetWindowDrawList();
  const char* text_end = FindRenderedTextEnd(label);
  ImVec2 text_size = CalcTextSize(label,text_end,true,false);
  ImRect clip_rect = ImRect(pos0,pos1s);
  drawl->AddRectFilled(pos0,pos1,hovered?color_hovered:(activetab?color_active:color));
  drawl->AddRect(pos0,pos1,color_active,0.0f,~0,1.0f);
  RenderTextClipped(pos0,pos1s,label,text_end,&text_size, ImVec2(0.5f,0.5f), &clip_rect);
  
  // draw the "x"
  if (p_open && size.x >= mintabwidth){
    drawl->AddLine(center+ImVec2(-crossz,-crossz), center+ImVec2(crossz,crossz),IsItemHovered()?cross_color_hovered:cross_color);
    drawl->AddLine(center+ImVec2( crossz,-crossz), center+ImVec2(-crossz,crossz),IsItemHovered()?cross_color_hovered:cross_color);
  }

  return clicked;
}

void ImGui::ResizeGripOther(const char *label, ImGuiWindow* window, ImGuiWindow* cwindow, bool *dclicked/*=nullptr*/){
  static bool first = true;
  if (dclicked) *dclicked = false;

  static ImVec2 pos_orig = {};
  static ImVec2 size_orig = {};
  static ImVec2 csize_orig = {};
  ImGuiContext *g = GetCurrentContext();
  const ImVec2 br = window->Rect().GetBR();
  ImDrawList* dl = window->DrawList;
  const float resize_corner_size = ImMax(g->FontSize * 1.35f, g->Style.WindowRounding + 1.0f + g->FontSize * 0.2f);
  const ImRect resize_rect(br - ImVec2(resize_corner_size * 0.75f, resize_corner_size * 0.75f), br);
  char tmp[strlen(label)+15];
  ImFormatString(tmp,IM_ARRAYSIZE(tmp),"%s__resize__",label);
  const ImGuiID resize_id = window->GetID(tmp);

  // no clipping; save previous clipping
  ImRect saverect = window->ClipRect;
  window->ClipRect = ImVec4(-FLT_MAX,-FLT_MAX,+FLT_MAX,+FLT_MAX);
  dl->PushClipRectFullScreen();

  // button behavior
  bool hovered, held;
  ButtonBehavior(resize_rect, resize_id, &hovered, &held, ImGuiButtonFlags_FlattenChildren);

  // update the static flags
  if (held){
    if (first) {
      pos_orig = window->Pos;
      size_orig = window->SizeFull;
      csize_orig = cwindow->SizeFull;
    }
    first = false;
  } else {
    first = true;
  }

  // mouse cursor
  if (hovered || held)
    g->MouseCursor = ImGuiMouseCursor_ResizeNWSE;

  // apply the size change
  if (g->HoveredWindow == window && held && g->IO.MouseDoubleClicked[0]){
    if (dclicked) *dclicked = true;
    ImVec2 size_auto_fit = ImClamp(cwindow->SizeContents + cwindow->WindowPadding, g->Style.WindowMinSize, 
                            ImMax(g->Style.WindowMinSize, g->IO.DisplaySize - g->Style.DisplaySafeAreaPadding));

    cwindow->SizeFull = size_auto_fit;
    ClearActiveID();
  } else if (held)
    cwindow->SizeFull = csize_orig + (g->IO.MousePos - g->ActiveIdClickOffset + resize_rect.GetSize() - pos_orig) - size_orig;
  cwindow->Size = cwindow->SizeFull;

  // resize grip (from imgui.cpp)
  ImU32 resize_col = GetColorU32(held ? ImGuiCol_ResizeGripActive : hovered ? ImGuiCol_ResizeGripHovered : ImGuiCol_ResizeGrip);
  dl->PathLineTo(br + ImVec2(-resize_corner_size, -window->WindowBorderSize));
  dl->PathLineTo(br + ImVec2(-window->WindowBorderSize, -resize_corner_size));
  dl->PathArcToFast(ImVec2(br.x - g->Style.WindowRounding - window->WindowBorderSize, br.y - g->Style.WindowRounding - window->WindowBorderSize), g->Style.WindowRounding, 0, 3);
  dl->PathFillConvex(resize_col);
  dl->PopClipRect();
  window->ClipRect = saverect;
}

bool ImGui::LiftGrip(const char *label, ImGuiWindow* window){
  ImGuiContext *g = GetCurrentContext();
  const ImVec2 bl = window->Rect().GetBL();
  ImDrawList* dl = window->DrawList;
  const float lift_corner_size = ImMax(g->FontSize * 1.35f, g->Style.WindowRounding + 1.0f + g->FontSize * 0.2f);
  const ImRect lift_rect(bl - ImVec2(0.f, lift_corner_size * 0.75f), bl + ImVec2(lift_corner_size * 0.75f, 0.f));
  char tmp[strlen(label)+15];
  ImFormatString(tmp,IM_ARRAYSIZE(tmp),"%s__lift__",label);
  const ImGuiID lift_id = window->GetID(tmp);

  // no clipping; save previous clipping
  ImRect saverect = window->ClipRect;
  window->ClipRect = ImVec4(-FLT_MAX,-FLT_MAX,+FLT_MAX,+FLT_MAX);
  dl->PushClipRectFullScreen();

  // button behavior
  bool hovered, held;
  ButtonBehavior(lift_rect, lift_id, &hovered, &held, ImGuiButtonFlags_FlattenChildren);

  // lift grip (from imgui.cpp's resize grip)
  ImU32 lift_col = GetColorU32(held ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
  dl->PathLineTo(bl + ImVec2(window->WindowBorderSize, -lift_corner_size));
  dl->PathLineTo(bl + ImVec2(lift_corner_size, -window->WindowBorderSize));
  dl->PathArcToFast(ImVec2(bl.x + g->Style.WindowRounding + window->WindowBorderSize, bl.y - g->Style.WindowRounding - window->WindowBorderSize), g->Style.WindowRounding, 3, 6);
  dl->PathFillConvex(lift_col);
  dl->PopClipRect();
  window->ClipRect = saverect;
  
  return held && IsMouseDragging();
}

void ImGui::AttachTooltip(const char* desc, float delay, float maxwidth){
  ImGuiContext *g = GetCurrentContext();
  ImGuiID id = g->CurrentWindow->DC.LastItemId;
  float time = GetTime();
  int thisframe = GetFrameCount();

  if (thisframe != tooltip_thisframe){
    // run once every frame, in the first call
    if (!tooltip_washovered){
      tooltip_time = time;
      tooltip_shownid = 0;
      tooltip_hoveredid = 0;
    }
    tooltip_thisframe = thisframe;
    tooltip_washovered = false;
  }

  if (g->HoveredId == id){
    // If no tooltip is being shown and the mouse moves from one tooltip element
    // to another, this is the same as if it moved from a zone without any
    // tooltip elements.
    if (id != tooltip_hoveredid && tooltip_shownid == 0)
      tooltip_time = time;

    if (tooltip_lastactive != 0.f)
      delay = fmin(delay,fmax(tooltip_time - tooltip_lastactive,0.f));

    tooltip_washovered = true;
    tooltip_hoveredid = id;

    if (time - tooltip_time > delay){
      tooltip_shownid = id;
      tooltip_lastactive = time;
      BeginTooltip();
      PushTextWrapPos(maxwidth);
      TextUnformatted(desc);
      PopTextWrapPos();
      EndTooltip();
    }
  }
}
