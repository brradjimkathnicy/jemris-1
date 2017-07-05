function [theStruct,Counter] = parseXMLseq(handles)
% convert XML file to a MATLAB structure.
% helper function of JEMRIS_seq.m

%
%  JEMRIS Copyright (C) 
%                        2006-2014  Tony Stoecker
%                        2007-2014  Kaveh Vahedipour
%                        2009-2014  Daniel Pflugfelder
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
MODULE_TYPE_COUNTER=[0 0 0 0];

try
   tree = xmlread(fullfile(handles.seqdir,handles.seqfile));

catch
   error('Failed to read XML file %s.',filename);
end

% Recurse over child nodes. This could run into problems 
% with very deeply nested trees.
%try
 if nargin == 2
   theStruct = parseChildNodes(tree,handles);
 else
   theStruct = parseChildNodes(tree);
 end
 theStruct.current=1;
%catch
   %error('Unable to parse XML file %s.');
%end


% ----- Subfunction PARSECHILDNODES -----
function children = parseChildNodes(theNode,handles)
% Recurse over node children.
children = [];
if theNode.hasChildNodes
   childNodes = theNode.getChildNodes;
   numChildNodes = childNodes.getLength;
   allocCell = cell(1, numChildNodes);
  
   children = struct(             ...
      'Name', allocCell, 'Attributes', allocCell,    ...
      'Data', allocCell, 'Children', allocCell, 'current', allocCell);
    i=0;
    for count = 1:numChildNodes
        theChild = childNodes.item(count-1);
        if nargin == 2 
            c = makeStructFromNode(theChild,handles);
        else
            c = makeStructFromNode(theChild);
        end
	if isempty(c)
		i=i+1;
	else
        	children(count-i) = c;
	end
    end
    if i>0; children(numChildNodes-i+1:numChildNodes)=[]; end
end

% ----- Subfunction MAKESTRUCTFROMNODE -----
function nodeStruct = makeStructFromNode(theNode,handles)
% Create structure of node info.
global MODULE_TYPE_COUNTER
if nargin==2
    PC=parseChildNodes(theNode,handles);
    s = upper(char(theNode.getNodeName));
    for i=1:length(handles.Modules)
         if strcmp(upper(handles.Modules{i}),s)
             switch upper(handles.ModType{i})
                 case 'CONCAT'
                     j=1;
                 case 'ATOM'
                     j=2;
                     if strcmp(upper(handles.Values{i}{1}(1:5)),'DELAY'),j=3;end
                 case 'PULSE'
                     j=4;
                 otherwise
                     disp(['unkown module: ',s])
             end
             MODULE_TYPE_COUNTER(j)=MODULE_TYPE_COUNTER(j)+1;
         end
    end
else
    PC=parseChildNodes(theNode);
end

nodeStruct = struct(                        ...
   'Name', char(theNode.getNodeName),       ...
   'Attributes', parseAttributes(theNode),  ...
   'Data', '',                              ...
   'Children', PC,    ...
   'current',0);

if any(strcmp(methods(theNode), 'getData'))
   nodeStruct.Data = char(theNode.getData); 
else
   nodeStruct.Data = '';
end
if strcmp(nodeStruct.Name,'#text'),nodeStruct=[];end

% ----- Subfunction PARSEATTRIBUTES -----
function attributes = parseAttributes(theNode)
% Create attributes structure.

attributes = [];
if theNode.hasAttributes
   theAttributes = theNode.getAttributes;
   numAttributes = theAttributes.getLength;
   allocCell = cell(1, numAttributes);
   attributes = struct('Name', allocCell, 'Value', ...
                       allocCell);

   for count = 1:numAttributes
      attrib = theAttributes.item(count-1);
      attributes(count).Name = char(attrib.getName);
      attributes(count).Value = char(attrib.getValue);
   end
end

