import React from "react";
import Sketch from './Sketch';
import "../css/LandingPage.css";

class ArenaMap extends React.Component {
  constructor(props){
    super(props);
  }

  render() {
    const arena = this.props.arena;
    return (
      <div className="Actual-Arena-Map">
          {Object.keys(arena).length !== 0 ? 
            <Sketch arena = {this.props.arena}/> 
            : 
            <div className="No-Arena-Map">No map loaded yet</div>}
      </div>
    );
  }
}

export default ArenaMap;
