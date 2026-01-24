import { Badge } from '@/components/ui/badge';
import { ConstructionIcon, WeightTildeIcon, WavesIcon, WifiIcon, HardDriveIcon, CheckIcon, XIcon, HelpCircleIcon, TvMinimalIcon } from 'lucide-react';

type Sensor = {
    name: string;
    status: "No connection" | "Active" | "Problem";
    espStatus?: string;
};

function getSensorIcon(sensorName: string, status: string) {
    const normalizedName = sensorName.toLowerCase().trim();

    const width = 12;
    const height = 12;
    const iconColor = status === 'Active' ? 'text-green-500' : status === 'Problem' ? 'text-red-500' : 'text-yellow-500';
    
    switch (normalizedName) {
        case 'esp main module':
            return <HardDriveIcon className={`w-${width} h-${height} ${iconColor}`} />;
        case 'ultrasonic sensor':
            return <WavesIcon className={`w-${width} h-${height} ${iconColor}`} />;
        case 'weight sensor':
            return <WeightTildeIcon className={`w-${width} h-${height} ${iconColor}`} />;
        case 'motor sensor':
            return <ConstructionIcon className={`w-${width} h-${height} ${iconColor}`} />;
        case 'wifi sensor':
            return <WifiIcon className={`w-${width} h-${height} ${iconColor}`} />;
        case 'oled display':
            return <TvMinimalIcon className={`w-${width} h-${height} ${iconColor}`} />;
        default:
            return <HelpCircleIcon className={`w-${width} h-${height} ${iconColor}`} />;
    }
}

function getStatusIcon(status: string) {
    switch (status) {
        case 'Active':
            return <CheckIcon className="w-8 h-8 text-white" />;
        case 'Problem':
            return <XIcon className="w-8 h-8 text-white" />;
        case 'No connection':
        default:
            return <HelpCircleIcon className="w-8 h-8 text-white" />;
    }
}

export function ModuleStatusCard({ sensor }: { sensor: Sensor }) {
    const statusColor = 
        sensor.status === 'Active' ? 'bg-green-500' :
        sensor.status === 'Problem' ? 'bg-red-500 ' :
        sensor.status === 'No connection' ? 'bg-yellow-500 ' :
        'bg-gray-600';

    return (
        <div className="grid grid-rows-3 aspect-square p-4 rounded-lg border bg-card/50 hover:bg-accent/50 transition-colors">
            <div className="flex items-center gap-4">
                <div className="p-2 bg-background rounded-full border shadow-sm text-foreground/80">
                    {getSensorIcon(sensor.name, sensor.status)}
                </div>
                <span className="font-semibold text-xl">
                    {sensor.name}
                </span>
            </div>
            <div className="text-sm mt-4">
                <span className="text-lg font-medium">Status:</span><br />
                <Badge
                    variant="default"
                    className={`${statusColor} text-xs flex items-center gap-1`}
                >
                    {getStatusIcon(sensor.status)}
                    {sensor.status}
                </Badge>
            </div>
            <div className="text-xs text-gray-600 mt-2">
                <span className="text-lg text-black font-medium">Additional info:</span><br />
                {sensor.espStatus || 'No additional info'}
            </div>
        </div>
    );
}