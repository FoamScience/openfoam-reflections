import React from 'react';

const ReflectJSONDisplay = ({ data, onModelChange }) => {
    const renderWidget = (key, value, description) => {
        if (Array.isArray(value)) {
            // Render a dropdown for arrays
            return (
                <div key={key}>
                    <label title={description}>{key}</label>
                    <select onChange={(e) => onModelChange(e.target.value)}>
                        {value.map((item, index) => (
                            <option key={index} value={item}>
                                {item}
                            </option>
                        ))}
                    </select>
                </div>
            );
        } else if (typeof value === 'string') {
            // Render an input for string values
            return (
                <div key={key}>
                    <label title={description}>{key}</label>
                    <input type="text" value={value} readOnly />
                </div>
            );
        } else {
            // Render JSON.stringify for other types
            return (
                <div key={key}>
                    <label title={description}>{key}</label>
                    <pre>{JSON.stringify(value)}</pre>
                </div>
            );
        }
    };

    return (
        <div>
            {Object.entries(data).map(([key, { description, value, _ }]) => renderWidget(key, value, description))}
        </div>
    );
};

export default ReflectJSONDisplay;
