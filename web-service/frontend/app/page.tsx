import { ParkingLotCard } from '@/components/dashboard/ParkingLotCard';
import { BarrierControlCard } from '@/components/dashboard/BarrierControlCard';
import { ActivityLogCard } from '@/components/dashboard/ActivityLogCard';
import { ModeToggle } from '@/components/mode-toggle';
import { DashboardProvider } from '@/components/dashboard-provider';

export default function Home() {
  return (
    <DashboardProvider>
      <div className="min-h-screen bg-slate-50 dark:bg-slate-950 p-4 sm:p-6 lg:p-8 font-[family-name:var(--font-geist-sans)]">
        <header className="mb-8 flex flex-row justify-between items-center gap-4">
          <div>
            <h1 className="text-3xl font-bold tracking-tight text-slate-900 dark:text-slate-100">Parking Dashboard</h1>
            <p className="text-muted-foreground">Real-time monitoring and control system</p>
          </div>
          <ModeToggle />
        </header>

        <main className="grid grid-cols-1 lg:grid-cols-4 gap-6 max-w-[1600px] mx-auto">
          {/* Left Column: Sensors & Controls */}
          <div className="lg:col-span-1 space-y-6 flex flex-col">
            <div className="flex-none">
              <BarrierControlCard />
            </div>
            <div className="flex-1 min-h-[300px]">
              <ActivityLogCard />
            </div>
          </div>

          {/* Right Column: Visualization (Takes up more space) */}
          <div className="lg:col-span-3 min-h-[500px] lg:h-auto">
            <ParkingLotCard />
          </div>
        </main>
      </div>
    </DashboardProvider>
  );
}
