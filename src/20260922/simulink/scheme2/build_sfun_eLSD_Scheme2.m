function build_sfun_eLSD_Scheme2()
%BUILD_SFUN_ELSD_SCHEME2 Generate and mex Scheme Two S-Functions (Legacy Code Tool).
%
%   Creates in ./generated:
%     sfun_eLSD_GetAswRx     Calls eLSD_Lib GetRx* through the flat wrapper
%     sfun_eLSD_SetAswTxMot  Calls eLSD_Lib SetTx* and mcIf setters
%     sfun_eLSD_StubInjectRx Host-only: loads simulated CAN into the stub
%     sfun_eLSD_StubReadTxMot Host-only: reads back last TX / motor commands
%
%   Host builds link the stub under ./stub. For target ERT builds, keep the Get
%   and Set S-Functions, drop the Stub* blocks, and replace the stub object with
%   the vendor eLSD_Lib in the Custom Code / packNGo settings.
%
%   Requires MATLAB R2020b with Simulink Coder (Legacy Code Tool) and a supported
%   C compiler (mex -setup).

here = fileparts(mfilename('fullpath'));
genDir = fullfile(here, 'generated');
if exist(genDir, 'dir') ~= 7
    mkdir(genDir);
end

% Headers eLSD_Lib.h / eLSD_Lib_ComTypes.h live in 20260922更新/
libHdr  = fileparts(fileparts(here));
incDir  = fullfile(here, 'include');
srcDir  = fullfile(here, 'src');
stubDir = fullfile(here, 'stub');

incPaths = {incDir, libHdr, stubDir};
srcPaths = {srcDir, stubDir};
commonSrc = {'eLSD_Sfun_FlatApi.c', 'eLSD_Lib_stub.c'};

old = cd(genDir);
cleanup = onCleanup(@() cd(old)); %#ok<NASGU>

defs = {};
defs{end+1} = localDef( ...
    'sfun_eLSD_GetAswRx', ...
    'void eLSD_Sfun_GetAswRx(single u1, single y1[40])', ...
    commonSrc, 'eLSD_Sfun_FlatApi.h', incPaths, srcPaths);

defs{end+1} = localDef( ...
    'sfun_eLSD_SetAswTxMot', ...
    'void eLSD_Sfun_SetAswTxMot(single u1[16], single u2[4])', ...
    commonSrc, 'eLSD_Sfun_FlatApi.h', incPaths, srcPaths);

defs{end+1} = localDef( ...
    'sfun_eLSD_StubInjectRx', ...
    'void eLSD_Sfun_StubInjectRx(single u1[40], single y1[1])', ...
    commonSrc, 'eLSD_Sfun_FlatApi.h', incPaths, srcPaths);

defs{end+1} = localDef( ...
    'sfun_eLSD_StubReadTxMot', ...
    'void eLSD_Sfun_StubReadTxMot(single y1[16], single y2[4])', ...
    commonSrc, 'eLSD_Sfun_FlatApi.h', incPaths, srcPaths);

for k = 1:numel(defs)
    def = defs{k};
    legacy_code('sfcn_cmex_generate', def);
    legacy_code('compile', def);
    legacy_code('slblock_generate', def);
    fprintf('Built %s\n', def.SFunctionName);
end

addpath(genDir);
fprintf('Scheme Two S-Functions are in %s\n', genDir);
end

function def = localDef(name, fcnSpec, srcFiles, hdr, incPaths, srcPaths)
def = legacy_code('initialize');
def.SFunctionName = name;
def.OutputFcnSpec = fcnSpec;
def.HeaderFiles   = {hdr};
def.SourceFiles   = srcFiles;
def.IncPaths      = incPaths;
def.SrcPaths      = srcPaths;
def.Options.language = 'C';
def.SampleTime = 0.005;
end
