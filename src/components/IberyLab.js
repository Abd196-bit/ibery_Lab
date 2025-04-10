"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || (function () {
    var ownKeys = function(o) {
        ownKeys = Object.getOwnPropertyNames || function (o) {
            var ar = [];
            for (var k in o) if (Object.prototype.hasOwnProperty.call(o, k)) ar[ar.length] = k;
            return ar;
        };
        return ownKeys(o);
    };
    return function (mod) {
        if (mod && mod.__esModule) return mod;
        var result = {};
        if (mod != null) for (var k = ownKeys(mod), i = 0; i < k.length; i++) if (k[i] !== "default") __createBinding(result, mod, k[i]);
        __setModuleDefault(result, mod);
        return result;
    };
})();
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const react_1 = __importStar(require("react"));
const react_split_pane_1 = __importDefault(require("react-split-pane"));
const material_1 = require("@mui/material");
const Editor_1 = __importDefault(require("./Editor"));
const Terminal_1 = __importDefault(require("./Terminal"));
const FileExplorer_1 = __importDefault(require("./FileExplorer"));
const IberyLab = () => {
    const [theme, setTheme] = (0, react_1.useState)('dark');
    const muiTheme = (0, material_1.createTheme)({
        palette: {
            mode: theme,
        },
    });
    return (<material_1.ThemeProvider theme={muiTheme}>
      <material_1.CssBaseline />
      <material_1.Box sx={{ display: 'flex', height: '100vh', width: '100vw' }}>
        <FileExplorer_1.default />
        <react_split_pane_1.default split="vertical" defaultSize="70%" style={{ position: 'relative' }}>
          <Editor_1.default />
          <Terminal_1.default />
        </react_split_pane_1.default>
      </material_1.Box>
    </material_1.ThemeProvider>);
};
exports.default = IberyLab;
