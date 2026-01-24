"use client";

import Spline from "@splinetool/react-spline";
import { useEffect, useRef } from "react";

export default function SplineComponent() {
    const splineContainerRef = useRef<HTMLDivElement>(null);

    useEffect(() => {
        function handleKeyDown(e: KeyboardEvent) {
            // Only allow simulated events to pass through
            if (!(e as any).isSimulated) {
                // This is a real keyboard event - block it
                e.stopPropagation();
                e.preventDefault();
                console.log('Real keyboard event blocked:', e.key);
                return;
            }
            
            console.log('Simulated event allowed:', e.key, 'Shift:', e.shiftKey);
        }

        function handleKeyUp(e: KeyboardEvent) {
            // Block real keyboard events for keyup too
            if (!(e as any).isSimulated) {
                e.stopPropagation();
                e.preventDefault();
                return;
            }
        }

        // Add listeners with capture phase to intercept before Spline
        document.addEventListener('keydown', handleKeyDown, true);
        document.addEventListener('keyup', handleKeyUp, true);
        
        return () => {
            document.removeEventListener('keydown', handleKeyDown, true);
            document.removeEventListener('keyup', handleKeyUp, true);
        };
    }, []);

    function onSplineKeyDown(e: any) {
        console.log('Spline key event:', e.target?.name);
    }

    return (
        <div ref={splineContainerRef} className="h-full">
            <Spline 
                scene="https://prod.spline.design/dSDrlBsl8wQPTGyl/scene.splinecode" 
                onSplineKeyDown={onSplineKeyDown} 
            />
        </div>
    );
}