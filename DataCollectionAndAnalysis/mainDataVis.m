%% *DSH Data Visualizer*
% _By: Joshua Cohen_
%start on a fresh template, init variables
clc
close all
clear all

%Specify how you would like to visualize the data with visScheme
%th = temp and humidity over time, all nodes
%t = Just temps, separated per node
%h = Just humidity, seprated per node
visScheme = "h";

fName = 'dshData.csv';
delim = ',';
%cannot be lees than 2 or more than the number of rows of data
%startRow = 153700;
startRow = 2;
WIDTH = 1300;
HEIGHT = 700;
%gets the size of the screen being used
screenSize = get(0,'ScreenSize');
%sets figure in center of screen
figureCoords = [(screenSize(3)-WIDTH)/2,(screenSize(4)-HEIGHT)/2,WIDTH,HEIGHT];
%create figure and set it's properties
mainFig = figure('Position',figureCoords);

%   column1: text (%s)
%	column2: double (%f)
%   column3: double (%f)
%	column4: text (%s)
%   column5: double (%f)
format = '%s%f%f%s%f%[^\n\r]';
%Open and store information from the file
fileID = fopen(fName,'r');
dataArray = textscan(fileID, format, ...
    'Delimiter', delim,...
    'EmptyValue' ,NaN,...
    'HeaderLines' ,startRow-1, ...
    'ReturnOnError', false, ...
    'EndOfLine', '\r\n');
fclose(fileID);

%Format and store the data in variables
Time = strtok(dataArray{:, 1},'.');
Time = string(Time);
NetworkID = dataArray{:, 2};
NodeID = dataArray{:, 3};
SensorType = dataArray{:, 4};
SensorValue = dataArray{:, 5};
dataLength = length(dataArray{1});
if (strcmp(visScheme, "th"))
    %pick an arbitrary column in the data, and preallocates for that length
    temps = zeros(1,dataLength);
    humids = zeros(1,dataLength);
    tTime = strings(1,dataLength);
    hTime = strings(1,dataLength);
    tCount = 1;
    hCount = 1;
    %Separates temp and humid data to be graphed separately
    for i = 1:dataLength
        if(strcmp(SensorType{i},'TEMPC'))
            temps(tCount) = SensorValue(i);
            tTime(tCount) = Time(i);
            tCount = tCount + 1;
        elseif(strcmp(SensorType{i},'HUM'))
            humids(hCount) = SensorValue(i);
            hTime(hCount) = Time(i);
            hCount = hCount+1;
        end
    end
    %format the time arrays for graphing
    tTime = char(tTime);
    tTime = squeeze(tTime(1,:,:))';
    hTime = char(hTime);
    hTime = squeeze(hTime(1,:,:))';
    
    %plot
    plot(datenum(tTime([1:tCount-1], :)),temps(1, [1:tCount-1]))
    datetick('x', 31);
    hold on;
    plot(datenum(hTime([1:hCount-1], :)),humids(1, [1:hCount-1]))
    
    %graph labels
    axis manual;
    axisTitle = title('Humidity and Temperature Values');
    set(axisTitle,'FontSize', 25);
    set(axisTitle, 'Color', 'k');
    legend('Temperatures (C^o)','Humidities', 'Location','southwest')
    xlabel('Date') % x-axis label
    ylabel('Value') % y-axis label
elseif (strcmp(visScheme, "t"))
    numNodes = max(NodeID);
    nodeTemps = zeros(numNodes, dataLength);
    nodeTimes = strings(numNodes,dataLength);
    nodeIdx = [1:numNodes;ones(1, numNodes)];
    legendInfo = strings(numNodes, 1);
    %Separates temp data from each node to be graphed separately
    for i = 1:dataLength
        %finds the temp data
        %if(strcmp(SensorType{i},'TEMPC') && (NodeID(i)==1 || NodeID(i)==3))
        if(strcmp(SensorType{i},'TEMPC'))
            %adds temp data to preallocated array index(NodeID,End of the
            %column)
            currNodeID = NodeID(i);
            nodeTemps(currNodeID,nodeIdx(2,currNodeID)) = SensorValue(i);
            nodeTimes(currNodeID,nodeIdx(2,currNodeID)) = Time(i);
            nodeIdx(2,currNodeID) = nodeIdx(2,currNodeID)+1;
        end
    end
         %format the time arrays for graphing
        nodeTimes = char(nodeTimes);
        %sizeVec = size(nodeTimes);
        nodeTimes = permute(nodeTimes,[1,3,2]);
        %plot
     hold on;
     indx = [];
    for i = 1:numNodes
        %plot
        if (nodeIdx(2,i) ~=1)
        plot(datenum(squeeze(nodeTimes(i,[1:nodeIdx(2,i)-1], :)), 31),nodeTemps(i,[1:nodeIdx(2,i)-1]))
        legendInfo(i) = "Node " + i;
        else
           indx = [indx, i]; 
        end
    end
    legendInfo(indx) = [];
    datetick('x', 31);

    %graph labels
    axis manual;
    axisTitle = title('Temperature by NodeID');
    set(axisTitle,'FontSize', 25);
    set(axisTitle, 'Color', 'k');
    legend(legendInfo, 'Location','northwest')
    xlabel('Date') % x-axis label
    ylabel('Value') % y-axis label
elseif (strcmp(visScheme, "h"))
    numNodes = max(NodeID);
    nodeHumids = zeros(numNodes, dataLength);
    nodeTimes = strings(numNodes,dataLength);
    nodeIdx = [1:numNodes;ones(1, numNodes)];
    legendInfo = strings(numNodes, 1);
    %Separates temp data from each node to be graphed separately
    for i = 1:dataLength
        %finds the temp data
        %if(strcmp(SensorType{i},'HUM') && NodeID(i)==6)
        if(strcmp(SensorType{i},'HUM'))
            %adds temp data to preallocated array index(NodeID,End of the
            %column)
            currNodeID = NodeID(i);
            nodeHumids(currNodeID,nodeIdx(2,currNodeID)) = SensorValue(i);
            nodeTimes(currNodeID,nodeIdx(2,currNodeID)) = Time(i);
            nodeIdx(2,currNodeID) = nodeIdx(2,currNodeID)+1;
        end
    end
         %format the time arrays for graphing
        nodeTimes = char(nodeTimes);
        %sizeVec = size(nodeTimes);
        nodeTimes = permute(nodeTimes,[1,3,2]);
        %plot
     hold on;
     indx = [];
    for i = 1:numNodes
        %plot
        if (nodeIdx(2,i) ~=1)
        plot(datenum(squeeze(nodeTimes(i,[1:nodeIdx(2,i)-1], :)), 31),nodeHumids(i,[1:nodeIdx(2,i)-1]))
        legendInfo(i) = "Node " + i;
        else
           indx = [indx, i]; 
        end
    end
    legendInfo(indx) = [];
    datetick('x', 31);

    %graph labels
    axis manual;
    axisTitle = title('Humidity by NodeID');
    set(axisTitle,'FontSize', 25);
    set(axisTitle, 'Color', 'k');
    legend(legendInfo, 'Location','northwest')
    xlabel('Date') % x-axis label
    ylabel('Value') % y-axis label
end
%clearvars fName delimiter startRow formatSpec fileID dataArray ans Time SensorType SensorValue NetworkID NodeID;