import React, { useEffect, useState } from 'react';
import logo from './logo.svg';
import './App.css';
import ReflectJSONDisplay from './ReflectJSONDisplay';
import axios from 'axios';

function App() {
    useEffect(() => {
        document.title = 'OpenFOAM Reflection';
    }, []);

    const [data, setData] = useState(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    const [endpoint, setEndpoint] = useState('http://0.0.0.0:18080/classes/mff__baseModel');

    const fetchData = (url) => {
        setLoading(true);
        setError(null);

        axios.get(url)
            .then(response => {
                setData(response.data);
                setLoading(false);
            })
            .catch(error => {
                console.error("Error fetching data:", error);
                setError(error);
                setLoading(false);
            });
    };

    useEffect(() => {
        fetchData(endpoint);
    }, [endpoint]);

    const [h1Text, setH1Text] = useState("Members for the mff::baseModel class");
    const [h2Text, setH2Text] = useState("using Endpoint http://0.0.0.0:18080/classes/mff__baseModel");

    const handleModelChange = (newModel) => {
        const newEndpoint = `http://0.0.0.0:18080/classes/mff__${newModel}`;
        setH1Text(`Members for the mff::${newModel} class`);
        setH2Text(`using Endpoint  ${newEndpoint}`);
        setEndpoint(newEndpoint);
    };

    if (loading) {
        return <div>Loading...</div>;
    }

    if (error) {
        return <div>Error: {error.message}. <b>http://0.0.0.0:18080/classes/mff__baseModel</b> must be accessible</div>;
    }

    return (
        <div className="App">
            <h1 id="classTitle">{h1Text}</h1>
            <h2 id="classEndpoint">{h2Text}</h2>
            {data && <ReflectJSONDisplay data={data} onModelChange={handleModelChange} />}
        </div>
    );
}

export default App;
