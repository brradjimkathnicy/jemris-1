function [S,x]=plotSeqTree(S,handles,x,y)
%recursevely draw the sequence tree
%helper function of JEMRIS_seq.m

%
%  JEMRIS Copyright (C) 
%                        2006-2015  Tony Stoecker
%                        2007-2015  Kaveh Vahedipour
%                        2009-2015  Daniel Pflugfelder
%                                  
%
%  This program is free software; you can redistribute it and/or modify
%  it under the terms of the GNU General Public License as published by
%  the Free Software Foundation; either version 2 of the License, or
%  (at your option) any later version.
%
%  This program is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU General Public License for more details.
%
%  You should have received a copy of the GNU General Public License
%  along with this program; if not, write to the Free Software
%  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
%
global MODULE_TYPE_COUNTER 

dy=.5; dx=.4; dr=.15; t=[0:.1:2*pi]; X=1.2*dr*cos(t); Y=1.2*dr*sin(t);

if nargin<3 %the root node (Paramter or ContainerSequence)
  if nargin==1,handles=[];end
  if strcmpi(S.Name,'PARAMETERS')
      x=1;y=1.5;C=[1 .7 .4];
  else
      x=0;y=1.5;C=[1 .4 .4];
  end
  set(gca,'visible','off','xlim',[-.1 .1],'ylim',[-.1 .1]);
  MODULE_TYPE_COUNTER=[0 0 0 0 0];
else
 if (x==1 && y==1),x=-1.5;y=1.5; end
 switch upper(S.Name)
    case 'CONCATSEQUENCE'
        C=[1 1 .4];  j=1;
    case 'CONTAINER'
        C=[1 .4 .4]; j=2;
    case 'ATOMICSEQUENCE'
        C=[.6 .6 1]; j=3;
    case'DELAYATOMICSEQUENCE'
        C=[.4 1 .4]; j=4;
    otherwise
        C=[1 1 1]; X=[-dr -dr dr dr]; Y=[-dr dr dr -dr]; j=5;
 end
 MODULE_TYPE_COUNTER(j)=MODULE_TYPE_COUNTER(j)+1;
end

% plot static atom to the right (second child of parameters)
if numel(handles.Seq(1).Children)>1 && strcmpi(handles.Seq(1).Name,'PARAMETERS')
 if strcmp(S.Attributes(1).Value,handles.Seq(1).Children(2).Attributes(1).Value)
    x=1;y=1;
    C=[.5 .5 1];
 end
end

%draw symbol
a=X+x;a=[a a(1)];b=Y+y;b=[b b(1)];
S.hp=patch(a,b,C);
S.hl=line(a,b,'color',[0 0 0],'linewidth',2);

%overlay icon on pulseshapes
if isempty([findstr('PARAMETER',upper(S.Name)) findstr('SEQUENCE',upper(S.Name)) findstr('CONTAINER',upper(S.Name))])
    try
     eval(['B=handles.icons.',upper(S.Name),';']);
    catch
     B=handles.icons.ANYPULSE;
    end

    B(find(isnan(B)))=1;
    S.hi=image('xdata',mean(a)+.6*dr*[-7:8]/8,'ydata',mean(b)+.6*dr*[8:-1:-7]/8,'cdata',B);
else
    S.hi=0;
end

%call all children
oldx=x;
N=length(S.Children);
for i=1:N
    S.Children(i).hp=0; S.Children(i).hl=0; S.Children(i).hi=0; S.Children(i).ht=0;
    %pulseshape tags are drawn at different positions ...
    if ~isempty(findstr('PULSE',upper(S.Children(i).Name)))
      S.Children(i)=plotSeqTree(S.Children(i),handles,x,y-i*(dy-.2));
    % ... than sequence tags (Concat, Atomic, Delay, ...)
    else
      [S.Children(i),x]=plotSeqTree(S.Children(i),handles,x,y-dy);
      h=S.Children(i).hp;
      %draw tree connection lines
      if ~strcmp(upper(S.Name),'PARAMETERS')
        line([mean(a) mean(get(h,'XData'))],...
             [min(b) max(get(h,'YData'))],'color',[0 0 0],'linewidth',2)
      end
    end
end

%draw 1st two chraracters of the node name to the symbol
ANAME='';
for i=1:length(S.Attributes); 
    if strcmp(upper(S.Attributes(i).Name),'NAME')
        ANAME=S.Attributes(i).Value;
    end
end
if length(ANAME)>3 , ANAME=ANAME(1:3); end

%draw a 'P' in Parameters 
%if strcmp(upper(S.Name),'PARAMETERS')
if isempty([findstr('PARAMETER',upper(S.Name)) findstr('SEQUENCE',upper(S.Name)) findstr('CONTAINER',upper(S.Name))])
    xs=0.07; ys=0.11; FS=10; fontcolor=[.8 0 0];
else
    xs=-0.04-0.01*length(ANAME); ys=-0.02; FS=14; fontcolor=[0 0 0];
end
S.ht=text(mean(a)+xs,mean(b)+ys,ANAME,'color',fontcolor,'fontsize',FS,'fontweight','bold');

%end

%increase x-position
if (x==oldx),x=x+dx;end

%finally, the root: 1.) sets minimal axis limits, 2.) updates the guidata, 
%                   3.) sets the ButtonDownFncs,  4.) calls the active module
if nargin<3
    x=[1 -1];y=[1 -1];
    [x,y]=refine_axis(S,x,y);
    x(2)=max([x(2) 1]); y(1)=min([y(1) 0]);
    set(gca,'xlim',x+[-.1 .1],'ylim',y+[-.1 .1]);
    handles.Seq=S;
    guidata(handles.output, handles);
    set_ButtonDownFnc(S,handles); 
    call_CurrentModule(S,handles);
    guidata(handles.output, handles);
end

%%%%%%%%%%END OF plotSeqTree %%%%%%%%%%

%local functions

%%%% find minimal axis limist recursively %%%%
function [x,y]=refine_axis(S,x,y)
 if S.hp==0;return;end
 x=[min([get(S.hp,'XData')' x(1)]) max([get(S.hp,'XData')' x(2)])];
 y=[min([get(S.hp,'YData')' y(1)]) max([get(S.hp,'YData')' y(2)])];
 for i=1:length(S.Children)
     [x,y]=refine_axis(S.Children(i),x,y);
 end

%%%% set current module recursively %%%%
function S=set_active(hp,S)
 S.current = (hp==S.hp);
 %if S.current,fprintf('%20s %d  %9.4f %9.4f %9.4f \n',S.Name,S.current,S.hp,hp,gco); end
 for i=1:length(S.Children)
     S.Children(i)=set_active(hp,S.Children(i));
 end

%%%% set button down function for all handles of this module %%%%
function set_ButtonDownFnc(S,handles)
 set(S.hp,'ButtonDownFcn',{@seqcad_common,S,handles}); %the path
 set(S.hl,'ButtonDownFcn',{@seqcad_common,S,handles}); %the line
 if S.hi ~= 0, set(S.hi,'ButtonDownFcn',{@seqcad_common,S,handles}); end %the image (pulse shape icon)
 if S.ht ~= 0, set(S.ht,'ButtonDownFcn',{@seqcad_common,S,handles}); end %the text (loop connection number)
 for i=1:length(S.Children)
     set_ButtonDownFnc(S.Children(i),handles);
 end

%call ButtonDownFnc of the active module
function call_CurrentModule(S,handles)
 if S.current
        seqcad_common([ ], [ ], S, handles);
        return
 end
 for i=1:length(S.Children)
     call_CurrentModule(S.Children(i),handles);
 end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% button press functions %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%all ButtonDownFnc's call this routine
function seqcad_common(src,eventdata,seq,handles) 
if isempty(handles),return,end
global CONTAINERSEQFILE OPEN_CONTAINERSEQUENCE

if ~strcmp(get(handles.figure1,'SelectionType'),'open')
    seqcad_common_single_click(src,eventdata,seq,handles)
else
    if strcmpi(seq.Name,'CONTAINER')
        container_seq_file=''; 
        for i=1:length(seq.Attributes)
            if strcmpi(seq.Attributes(i).Name,'FILENAME')
                container_seq_file = seq.Attributes(i).Value;
            end
        end
        CONTAINERSEQFILE = fullfile(handles.seqdir,container_seq_file);
        if OPEN_CONTAINERSEQUENCE
            JEMRIS_ContainerSequence;
        else
            seqcad_common_single_click(src,eventdata,seq,handles)
        end
    else
        seqcad_common_single_click(src,eventdata,seq,handles)
    end
end


function seqcad_common_single_click(src,eventdata,seq,handles) 
handles.Seq=set_active(seq.hp,handles.Seq);
guidata(handles.output, handles);
h=findobj(gca,'Type','Line');
for i=1:length(h), set(h,'color',[0 0 0],'linewidth',2), end
set(seq.hl,'color',[1 0 0],'linewidth',3)
global HANDLES OPEN_CONTAINERSEQUENCE
OPEN_CONTAINERSEQUENCE=1;
HANDLES=handles;
%show attributes
if strcmp(upper(seq.Name),'PARAMETERS') 
    set(handles.SeqObjectPanel,'Title','Module: Parameters')
    A=handles.Parameter;
    Adisp=A;
    HA=handles.ParameterHidden;
else
    set(handles.SeqObjectPanel,'Title',['Module: ',seq.Name])
    A=handles.Attributes{find(strcmp(handles.Modules,upper(seq.Name)))};
    Adisp=A;
    HA=handles.HiddenAttributes{find(strcmp(handles.Modules,upper(seq.Name)))};
    % Replace strings with display names
    for i=1:length(A)
        idx=strcmpi(A{i},{seq.Attributes.Name});
        if any(idx)
            Adisp{i}=seq.Attributes(idx).DispName;
        end
    end
    for i=1:length(HA)
        idx=strcmpi(HA{i},{seq.Attributes.Name});
        if any(idx)
            HA{i}=seq.Attributes(idx).DispName;
        end
    end
end

if isempty(seq.Attributes)
    seq.Attributes.Name='Name';
    seq.Attributes.DispName=seq.Attributes.Name;
    seq.Attributes.Value=seq.Name;
end

for i=1:20
    if i>length(A)
        bvis='''off''';
    else
        set(handles.(['SOtag',num2str(i)]),'String',Adisp{i});
        set(handles.(['SOEtag',num2str(i)]),'UserData',A{i});   % Store actual name in for edit callback
        s=struct2cell(seq.Attributes);
        n=find(strcmp(A{i},squeeze(s(1,:,:))));
        if isempty(n)
            val='';
            if strcmp(A{i},'Name'),val=seq.Name;end
        else
            val=seq.Attributes(n).Value;
        end
        eval(['set(handles.SOEtag',num2str(i),',''String'',''',val,''');'])
        bvis='''on''';
    end
    eval(['set(handles.SOtag',num2str(i),',''Visible'',',bvis,');'])
    eval(['set(handles.SOEtag',num2str(i),',''Visible'',',bvis,');'])
end
if isempty(HA)
 set(handles.HiddenAttr,'Visible','off');
else
 set(handles.HiddenAttr,'Visible','on');
 HA(2:end+1)=HA; HA{1}=' list of hidden attributes';
 for i=2:length(HA),HA{i}=strrep(HA{i},'HIDDEN','');end
 set(handles.HiddenAttr,'String',HA);
end

%change the sequence tree?
global INSERT_MODULE_NUMBER % -3 = copy, -2 = swap, -1 =delete, 1,2,... insert modules
ism=INSERT_MODULE_NUMBER;
if ism
  Seq=changeSeqTree(handles.Seq,handles,1);
  %if the tree is changed, update the handles and redraw the tree
  if ~isempty(Seq) && ism~=INSERT_MODULE_NUMBER 
    if ism==-1, Seq.current=1; end %case delete: highlight the root node
    handles.Seq=Seq;
    axes(handles.hax{7}); cla(handles.hax{7},'reset');
    handles.Seq=plotSeqTree(handles.Seq,handles);
    guidata(handles.output, handles);
  end
end

