import React, { useState } from 'react';
import SplitPane from 'react-split-pane';
import { Box, ThemeProvider, createTheme, CssBaseline } from '@mui/material';
import Editor from './Editor';
import Terminal from './Terminal';
import FileExplorer from './FileExplorer';

const IberyLab: React.FC = () => {
  const [theme, setTheme] = useState<'light' | 'dark'>('dark');

  const muiTheme = createTheme({
    palette: {
      mode: theme,
    },
  });

  return (
    <ThemeProvider theme={muiTheme}>
      <CssBaseline />
      <Box sx={{ display: 'flex', height: '100vh', width: '100vw' }}>
        <FileExplorer />
        <SplitPane
          split="vertical"
          defaultSize="70%"
          style={{ position: 'relative' }}
        >
          <Editor />
          <Terminal />
        </SplitPane>
      </Box>
    </ThemeProvider>
  );
};

export default IberyLab; 