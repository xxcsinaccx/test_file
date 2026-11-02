function build_VTC_Scheme2_R2020b()
%BUILD_VTC_SCHEME2_R2020B Build Scheme Two S-Function models (R2020b).
%
%   1) Builds the Legacy Code Tool S-Functions (stub-linked for host).
%   2) Builds VTC_Scheme2_Loopback_R2020b.slx
%        Proves Inject -> GetRx -> SetTxMot -> Readback without the controller.
%   3) Builds VTC_ASW_Scheme2_R2020b.slx when ../VTC_ASW_R2020b.slx exists
%        Wraps the Scheme One ASW Model block with Get/Set S-Functions so the
%        generated step calls eLSD_Lib through the flat wrappers.
%
%   Run build_VTC_TorqueVectoring_R2020b in the parent folder first if you want
%   the full ASW Scheme Two model.

here = fileparts(mfilename('fullpath'));
parent = fileparts(here);
addpath(here);
addpath(fullfile(here, 'generated'));
addpath(parent);

build_sfun_eLSD_Scheme2();

buildLoopback(fullfile(here, 'VTC_Scheme2_Loopback_R2020b'));

aswPath = fullfile(parent, 'VTC_ASW_R2020b.slx');
busMat  = fullfile(parent, 'VTC_Interface_Buses.mat');
if exist(aswPath, 'file') == 2 && exist(busMat, 'file') == 2
    S = load(busMat);
    assignin('base', 'RxBus', S.RxBus);
    assignin('base', 'ExtInBus', S.ExtInBus);
    assignin('base', 'TxBus', S.TxBus);
    assignin('base', 'MotorCmdBus', S.MotorCmdBus);
    buildAswScheme2(fullfile(here, 'VTC_ASW_Scheme2_R2020b'), 'VTC_ASW_R2020b');
    fprintf('Built VTC_ASW_Scheme2_R2020b (references VTC_ASW_R2020b)\n');
else
    warning(['VTC_ASW_R2020b.slx or VTC_Interface_Buses.mat not found in %s. ', ...
        'Loopback model only. Run build_VTC_TorqueVectoring_R2020b first.'], parent);
end

fprintf('Scheme Two build finished in %s\n', here);
end

%% ------------------------------------------------------------------------
function buildLoopback(mdlPath)
[folder, mdl] = fileparts(mdlPath);
if bdIsLoaded(mdl), close_system(mdl, 0); end
if exist([mdlPath '.slx'], 'file') == 2, delete([mdlPath '.slx']); end

new_system(mdl);
open_system(mdl);
set_param(mdl, ...
    'Solver', 'FixedStepDiscrete', ...
    'FixedStep', '0.005', ...
    'StopTime', '1.0', ...
    'SaveFormat', 'StructureWithTime');

add_block('simulink/Sources/In1', [mdl '/RxIn']);
add_block('simulink/Sources/In1', [mdl '/TxIn']);
add_block('simulink/Sources/In1', [mdl '/MotIn']);
add_block('simulink/Sinks/Out1', [mdl '/RxOut']);
add_block('simulink/Sinks/Out1', [mdl '/TxOut']);
add_block('simulink/Sinks/Out1', [mdl '/MotOut']);

set_param([mdl '/RxIn'], 'PortDimensions', '40', 'OutDataTypeStr', 'single', 'Position', [30 40 60 60]);
set_param([mdl '/TxIn'], 'PortDimensions', '16', 'OutDataTypeStr', 'single', 'Position', [30 140 60 160]);
set_param([mdl '/MotIn'], 'PortDimensions', '4', 'OutDataTypeStr', 'single', 'Position', [30 200 60 220]);
set_param([mdl '/RxOut'], 'Position', [520 40 550 60]);
set_param([mdl '/TxOut'], 'Position', [520 140 550 160]);
set_param([mdl '/MotOut'], 'Position', [520 200 550 220]);

addSfun(mdl, 'Inject', 'sfun_eLSD_StubInjectRx', [160 30 260 80]);
addSfun(mdl, 'GetRx', 'sfun_eLSD_GetAswRx', [320 30 420 80]);
addSfun(mdl, 'SetTxMot', 'sfun_eLSD_SetAswTxMot', [160 130 280 230]);
addSfun(mdl, 'Readback', 'sfun_eLSD_StubReadTxMot', [340 130 460 230]);

add_line(mdl, 'RxIn/1', 'Inject/1', 'autorouting', 'on');
add_line(mdl, 'Inject/1', 'GetRx/1', 'autorouting', 'on');
add_line(mdl, 'GetRx/1', 'RxOut/1', 'autorouting', 'on');
add_line(mdl, 'TxIn/1', 'SetTxMot/1', 'autorouting', 'on');
add_line(mdl, 'MotIn/1', 'SetTxMot/2', 'autorouting', 'on');
add_line(mdl, 'Readback/1', 'TxOut/1', 'autorouting', 'on');
add_line(mdl, 'Readback/2', 'MotOut/1', 'autorouting', 'on');

% Set before Readback in the same step (no data dependency between them).
set_param([mdl '/SetTxMot'], 'Priority', '1');
set_param([mdl '/Readback'], 'Priority', '2');

save_system(mdl, fullfile(folder, [mdl '.slx']));
close_system(mdl, 0);
fprintf('Built %s\n', mdl);
end

%% ------------------------------------------------------------------------
function buildAswScheme2(mdlPath, aswName)
[folder, mdl] = fileparts(mdlPath);
if bdIsLoaded(mdl), close_system(mdl, 0); end
if exist([mdlPath '.slx'], 'file') == 2, delete([mdlPath '.slx']); end

new_system(mdl);
open_system(mdl);
set_param(mdl, ...
    'Solver', 'FixedStepDiscrete', ...
    'FixedStep', '0.005', ...
    'StopTime', '10.0', ...
    'SaveFormat', 'StructureWithTime');

try
    set_param(mdl, 'SystemTargetFile', 'ert.tlc');
catch
end

% Root ports: ExtInBus + Sync (from stub inject in harness, or Constant on target)
add_block('simulink/Sources/In1', [mdl '/ExtInBus']);
add_block('simulink/Sources/In1', [mdl '/Sync']);
add_block('simulink/Sinks/Out1', [mdl '/TxBus']);
add_block('simulink/Sinks/Out1', [mdl '/MotorCmdBus']);
set_param([mdl '/ExtInBus'], 'OutDataTypeStr', 'Bus: ExtInBus', 'Position', [30 180 60 200]);
set_param([mdl '/Sync'], 'OutDataTypeStr', 'single', 'PortDimensions', '1', 'Position', [30 60 60 80]);
set_param([mdl '/TxBus'], 'OutDataTypeStr', 'Bus: TxBus', 'Position', [760 100 790 120]);
set_param([mdl '/MotorCmdBus'], 'OutDataTypeStr', 'Bus: MotorCmdBus', 'Position', [760 180 790 200]);

addSfun(mdl, 'GetRx', 'sfun_eLSD_GetAswRx', [120 40 220 100]);
add_line(mdl, 'Sync/1', 'GetRx/1', 'autorouting', 'on');

rxNames = localRxNames();
txNames = localTxNames();
motNames = localMotNames();

vectorToBus(mdl, rxNames, 'RxBus', 'RX', [280 30 420 140]);
add_line(mdl, 'GetRx/1', 'RX_Demux/1', 'autorouting', 'on');

add_block('simulink/Ports & Subsystems/Model', [mdl '/VTC_ASW']);
set_param([mdl '/VTC_ASW'], 'ModelName', aswName, 'Position', [480 80 620 200]);
add_line(mdl, 'RX_Bus/1', 'VTC_ASW/1', 'autorouting', 'on');
add_line(mdl, 'ExtInBus/1', 'VTC_ASW/2', 'autorouting', 'on');

% Log buses at the root and feed the same signals into SetTxMot.
add_line(mdl, 'VTC_ASW/1', 'TxBus/1', 'autorouting', 'on');
add_line(mdl, 'VTC_ASW/2', 'MotorCmdBus/1', 'autorouting', 'on');

busToVector(mdl, txNames, 'TX', [500 240 640 340]);
busToVector(mdl, motNames, 'MC', [500 360 640 430]);
add_line(mdl, 'VTC_ASW/1', 'TX_Select/1', 'autorouting', 'on');
add_line(mdl, 'VTC_ASW/2', 'MC_Select/1', 'autorouting', 'on');

addSfun(mdl, 'SetTxMot', 'sfun_eLSD_SetAswTxMot', [700 280 820 360]);
set_param([mdl '/SetTxMot'], 'Priority', '2');
add_line(mdl, 'TX_Mux/1', 'SetTxMot/1', 'autorouting', 'on');
add_line(mdl, 'MC_Mux/1', 'SetTxMot/2', 'autorouting', 'on');

save_system(mdl, fullfile(folder, [mdl '.slx']));
close_system(mdl, 0);
end

%% ------------------------------------------------------------------------
function addSfun(mdl, name, sfun, pos)
% Prefer the LCT-generated block if present; otherwise a raw S-Function block.
libBlk = ['sflib_unused/' sfun]; %#ok<NASGU>
try
    % LCT slblock_generate drops a block into a new model; use generic S-Function.
    add_block('simulink/User-Defined Functions/S-Function', [mdl '/' name]);
catch
    add_block('built-in/S-Function', [mdl '/' name]);
end
set_param([mdl '/' name], 'FunctionName', sfun, 'Position', pos);
end

function vectorToBus(sys, names, busName, tag, pos)
n = numel(names);
x = pos(1); y = pos(2);
add_block('simulink/Signal Routing/Demux', [sys '/' tag '_Demux']);
set_param([sys '/' tag '_Demux'], 'Outputs', num2str(n), 'Position', [x y x+30 y+20*n]);
add_block('simulink/Signal Routing/Bus Creator', [sys '/' tag '_Bus']);
set_param([sys '/' tag '_Bus'], ...
    'Inputs', num2str(n), ...
    'OutDataTypeStr', ['Bus: ' busName], ...
    'NonVirtualBus', 'on', ...
    'Position', [x+100 y x+140 y+20*n]);
for k = 1:n
    h = add_line(sys, sprintf('%s_Demux/%d', tag, k), sprintf('%s_Bus/%d', tag, k), ...
        'autorouting', 'on');
    try
        set_param(h, 'Name', names{k});
    catch
    end
end
end

function busToVector(sys, names, tag, pos)
n = numel(names);
x = pos(1); y = pos(2);
add_block('simulink/Signal Routing/Bus Selector', [sys '/' tag '_Select']);
set_param([sys '/' tag '_Select'], ...
    'OutputSignals', strjoin(names, ','), ...
    'Position', [x y x+40 y+20*n]);
add_block('simulink/Signal Routing/Mux', [sys '/' tag '_Mux']);
set_param([sys '/' tag '_Mux'], 'Inputs', num2str(n), 'Position', [x+80 y x+110 y+20*n]);
for k = 1:n
    add_line(sys, sprintf('%s_Select/%d', tag, k), sprintf('%s_Mux/%d', tag, k), ...
        'autorouting', 'on');
end
end

function names = localRxNames()
names = { ...
    'ECM2_AccelPedalPosn','ECM2_AccelPedalVldty','ECM2_Expired', ...
    'CSA2_SteerWheelAng','CSA2_SteerWheelAngSign','CSA2_SteerWheelAngStatus','CSA2_Expired', ...
    'ESP2_DrivingModReq','ESP2_DiagActv','ESP2_TAB_Sts','ESP2_Expired', ...
    'ESP1_ClutOpenReq','ESP1_Expired', ...
    'ABS3_VehSpd','ABS3_VehSpdVld','ABS3_VehSpdStatus','ABS3_ABSActv','ABS3_Expired', ...
    'ABM2_VehYawRate','ABM2_VehYawRateStatus','ABM2_VehDynYawRateVld','ABM2_Expired', ...
    'ABS1_FLWheelSpd','ABS1_FRWheelSpd','ABS1_FLWheelSpdStatus','ABS1_FRWheelSpdStatus','ABS1_Expired', ...
    'ABS2_RLWheelSpd','ABS2_RRWheelSpd','ABS2_RLWheelSpdStatus','ABS2_RRWheelSpdStatus','ABS2_Expired', ...
    'BCM6_RearELDLckSwtReq','BCM6_Expired', ...
    'VTC_RR1_EstimatedTorque','VTC_RR1_EstimatedTorqueStatus','VTC_RR1_ActuatorFailSts', ...
    'VTC_RR1_CouplingFaultSts','VTC_RR1_CouplingThermWarning','VTC_RR1_Expired'};
end

function names = localTxNames()
names = { ...
    'RL1_TrqReq_R','RL1_TrqReq_RStatus','RL1_OpenReq_R', ...
    'TOD1_AWDOperMod','TOD1_EstimatedTorque','TOD1_EstimatedTorqueStatus', ...
    'TOD1_CouplingFaultSts','TOD1_CouplingThermWarning','TOD1_ActuatorFailSts', ...
    'TOD1_ExternalReqIgnored','TOD1_DiagLmpReq', ...
    'ELD1_RearELDLckSts','ELD1_ELDFaultSts','ELD1_ELDFault', ...
    'ELD1_RollingCounter','ELD1_Checksum'};
end

function names = localMotNames()
names = {'MotTorqueCmd','MotTorqueLimit','MotCtrlEnable','MotCtrlMode'};
end
